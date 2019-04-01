#include <linux/fs.h>                
#include <linux/genhd.h>            
#include <linux/module.h>            
#include <linux/kernel.h>           
#include <linux/blkdev.h>           
#include <linux/bio.h>              
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/version.h>
#include <linux/hdreg.h>            
#include <linux/vmalloc.h>
#include <linux/string.h>
#include <linux/errno.h>


#define KERNEL_SECTOR_SIZE 512      
#define NSECTORS 1024                
#define MYDISK_MINORS 3                
#define BV_PAGE(bv) ((bv).bv_page)
#define BV_OFFSET(bv) ((bv).bv_offset)
#define BV_LEN(bv) ((bv).bv_len)

#define ARRY_SIZE(a) (sizeof(a) / sizeof(*a))
#define SECTOR_SIZE 512
#define MBR_SIZE SECTOR_SIZE
#define MBR_DISK_SIGNATURE_OFFSET 440
#define MBR_DISK_SIGNATURE_SIZE 4
#define PARTITION_TABLE_OFFSET 446
#define PARTITION_ENTRY_SIZE 16
#define PARTITION_TABLE_SIZE 64 
#define MBR_SIGNATURE_OFFSET 510
#define MBR_SIGNATURE_SIZE 2
#define MBR_SIGNATURE 0xAA55
#define BR_SIZE SECTOR_SIZE
#define BR_SIGNATURE_OFFSET 510
#define BR_SIGNATURE_SIZE 2
#define BR_SIGNATURE 0xAA55

void copy_mbr_n_br(u8 *disk);

typedef struct
{
    unsigned char boot_type; 
    unsigned char start_head;
    unsigned char start_sec:6;
    unsigned char start_cyl_hi:2;
    unsigned char start_cyl;
    unsigned char part_type;
    unsigned char end_head;
    unsigned char end_sec:6;
    unsigned char end_cyl_hi:2;
    unsigned char end_cyl;
    unsigned int abs_start_sec;
    unsigned int sec_in_part;
} PartEntry;

typedef PartEntry PartTable[4];

static PartTable def_part_table =
{
    {
        boot_type: 0x00,
        start_head: 0x00,
        start_sec: 0x2,
        start_cyl: 0x00,
        part_type: 0x83,
        end_head: 0x00,
        end_sec: 0x20,
        end_cyl: 0x09,
        abs_start_sec: 0x00000001,
        sec_in_part: 0x000001FF
    },
    {
        boot_type: 0x00,
        start_head: 0x00,
        start_sec: 0x1,
        start_cyl: 0x0A,
        part_type: 0x83,
        end_head: 0x00,
        end_sec: 0x20,
        end_cyl: 0x1F,
        abs_start_sec: 0x00000200,
        sec_in_part: 0x00000200
    }
};
static unsigned int def_log_part_br_cyl[] = {0x0A, 0x0E};
static const PartTable def_log_part_table[] =
{
    {
        {
            boot_type: 0x00,
            start_head: 0x00,
            start_sec: 0x2,
            start_cyl: 0x0A,
            part_type: 0x83,
            end_head: 0x00,
            end_sec: 0x20,
            end_cyl: 0x0D,
            abs_start_sec: 0x00000001,
            sec_in_part: 0x0000007F
        },

    
        {
            boot_type: 0x00,
            start_head: 0x00,
            start_sec: 0x2,
            start_cyl: 0x0E,
            part_type: 0x83,
            end_head: 0x00,
            end_sec: 0x20,
            end_cyl: 0x11,
            abs_start_sec: 0x00000001,
            sec_in_part: 0x0000007F
        }

    
    }
};
static void copy_mbr(u8 *disk)
{
    memset(disk, 0x0, MBR_SIZE);
    *(unsigned long *)(disk + MBR_DISK_SIGNATURE_OFFSET) = 0x36E5756D;
    memcpy(disk + PARTITION_TABLE_OFFSET, &def_part_table, PARTITION_TABLE_SIZE);
    *(unsigned short *)(disk + MBR_SIGNATURE_OFFSET) = MBR_SIGNATURE;
}
static void copy_br(u8 *disk, int start_cylinder, const PartTable *part_table)
{
    disk += (start_cylinder * 32 /* sectors / cyl */ * SECTOR_SIZE);
    memset(disk, 0x0, BR_SIZE);
    memcpy(disk + PARTITION_TABLE_OFFSET, part_table,
        PARTITION_TABLE_SIZE);
    *(unsigned short *)(disk + BR_SIGNATURE_OFFSET) = BR_SIGNATURE;
}
void copy_mbr_n_br(u8 *disk)
{
    int i;

    copy_mbr(disk);
    for (i = 0; i < ARRY_SIZE(def_log_part_table); i++)
    {
        copy_br(disk, def_log_part_br_cyl[i], &def_log_part_table[i]);
    }
}


static u_int mydisk_major = 0;
u8 *dev_data;
struct request *req;

static struct mydisk_device
{
    int size;
    u8 *data;
    spinlock_t lock;
    struct request_queue *queue;
    struct gendisk *gd;
}mydisk_dev;

static int mydisk_open(struct block_device *bdev, fmode_t mode)
{
    printk(KERN_INFO "Mydisk: Device opened\n");
    return 0;
}

static void mydisk_close(struct gendisk *disk, fmode_t mode)
{
    printk(KERN_INFO "Mydisk: Device closed\n");
}



static int mydisk_transfer(struct request *rq)
{

    int dir = rq_data_dir(rq);
    sector_t start_sector = blk_rq_pos(rq);

    struct bio_vec bv;

    struct req_iterator iter;

    unsigned int sectors;
    u8 *buffer;

    int ret = 0;
    
    rq_for_each_segment(bv, rq, iter)
    {
    
        buffer = page_address(BV_PAGE(bv)) + BV_OFFSET(bv);

        if (BV_LEN(bv) %KERNEL_SECTOR_SIZE != 0)
        {
            ret = -EIO;
        }

        sectors = BV_LEN(bv) / KERNEL_SECTOR_SIZE;

        if (dir == WRITE) 
        {
            
            memcpy(dev_data + (start_sector)*KERNEL_SECTOR_SIZE, buffer,sectors * KERNEL_SECTOR_SIZE);
        }
        else 
        {
            
            memcpy(buffer, dev_data + (start_sector)*KERNEL_SECTOR_SIZE,sectors * KERNEL_SECTOR_SIZE);
        }

    }


    return 0;
}

void mydisk_request(struct request_queue *queue)
{

    struct request *req;
    int ret;

    while ((req = blk_fetch_request(queue)) != NULL) 
    {    

          ret=mydisk_transfer(req);   
        
        __blk_end_request_all(req,ret);   

    }    

}

static struct block_device_operations mydisk_fops =
{
    .owner = THIS_MODULE,
    .open = mydisk_open,
    .release = mydisk_close,
};




static int __init mydisk_init(void)
{
    dev_data = vmalloc(NSECTORS * KERNEL_SECTOR_SIZE);   
    
    copy_mbr_n_br(dev_data);              

    mydisk_dev.size = NSECTORS;             

            
    
    mydisk_major = register_blkdev(mydisk_major, "dof");       
    
    if (mydisk_major <= 0) 
    {
        printk(KERN_ALERT "Unable to get major number\n");
        return -EBUSY;
    }
    

    
    spin_lock_init(&mydisk_dev.lock);     
    
    mydisk_dev.queue = blk_init_queue(mydisk_request, &mydisk_dev.lock);        

    

    if (mydisk_dev.queue == NULL)
        {
            printk(KERN_ERR "blk_init_queue failure\n");    
            return -ENOMEM;                
        }

    
    mydisk_dev.gd = alloc_disk(MYDISK_MINORS);     

    if (!mydisk_dev.gd) 
    {
        printk (KERN_NOTICE "alloc_disk failure\n");

    }
    
    mydisk_dev.gd->major = mydisk_major;    
    
    mydisk_dev.gd->first_minor = 0;      
    
    mydisk_dev.gd->fops = &mydisk_fops;            

    mydisk_dev.gd->queue = mydisk_dev.queue;

    mydisk_dev.gd->private_data = &mydisk_dev;

    sprintf(mydisk_dev.gd->disk_name,"dof");

    set_capacity(mydisk_dev.gd, mydisk_dev.size);

    add_disk(mydisk_dev.gd);

    printk(KERN_INFO "Block Drive Initialised");

    
    return 0;
    
    
}


static void __exit mydisk_exit(void) 
{
    del_gendisk(mydisk_dev.gd);
    put_disk(mydisk_dev.gd);
    blk_cleanup_queue(mydisk_dev.queue);
    vfree(dev_data);     
    unregister_blkdev(mydisk_major,"dof");
}    

module_init(mydisk_init);
module_exit(mydisk_exit);


MODULE_AUTHOR ( "Devansh Shah<h20180142@goa.bits-pilani.ac.in>" );
MODULE_LICENSE ( "GPL" );

