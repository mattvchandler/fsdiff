#ifndef __EXT2_H__
#define __EXT2_H__
//ext2.h
//Copyright 2012 Matthew Chandler
//CS 680 Project
//11-1-11

//ext2 information from http://www.nongnu.org/ext2-doc/ext2.html
//header for ext2 data structures and fs class

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstring>
//The following header allows use of [u]intN_t types for guaranteed length integer types
//we rely on these for portability as ext2 structures are fixed-width
//however, it would appear that the library is a new feature and g++ 4.6.1 requires the -std=c++0x flag to use it
//MSVC 2010+ will allow it by default
#include <cstdint>

struct Superblock
{
    uint32_t s_inodes_count;
    uint32_t s_blocks_count;
    uint32_t s_r_blocks_count;
    uint32_t s_free_blocks_count;
    uint32_t s_free_inodes_count;
    uint32_t s_first_data_block;
    uint32_t s_log_block_size;
    uint32_t s_log_frag_size;
    uint32_t s_blocks_per_group;
    uint32_t s_frags_per_group;
    uint32_t s_inodes_per_group;
    int32_t s_mtime;
    int32_t s_wtime;
    uint16_t s_mnt_count;
    uint16_t s_max_mnt_count;
    uint16_t s_magic;
    uint16_t s_state;
    uint16_t s_errors;
    uint16_t s_minor_rev_level;
    int32_t s_lastcheck;
    int32_t s_checkinterval;
    uint32_t s_creator_os;
    uint32_t s_rev_level;
    uint16_t s_def_resuid;
    uint16_t s_def_resgid;
    //-- EXT2_DYNAMIC_REV Specific --
    uint32_t s_first_ino;
    uint16_t s_inode_size;
    uint16_t s_block_group_nr;
    uint32_t s_feature_compat;
    uint32_t s_feature_incompat;
    uint32_t s_feature_ro_compat;
    uint8_t s_uuid[16];
    uint8_t s_volume_name[16];
    uint8_t s_last_mounted[64];
    uint32_t s_algo_bitmap;
    //-- Performance Hints -- unused in ext2
    uint8_t s_prealloc_blocks;
    uint8_t s_prealloc_dir_blocks;
    uint16_t __alignement0;
    //-- Journaling Support -- unused in ext2
    uint8_t s_journal_uuid[16];
    uint32_t s_journal_inum;
    uint32_t s_journal_dev;
    uint32_t s_last_orphan;
    //-- Directory Indexing Support -- unused in linux implementation of ext2
    uint32_t s_hash_seed[4];
    uint8_t s_def_hash_version;
    uint8_t __alignment1[3];
    //-- Other options --
    uint32_t s_default_mount_options;
    uint32_t s_first_meta_bg;
    uint8_t reserved[760];

    //byte-for-byte comparisons
    inline bool operator==(const Superblock & a) const
    {
        return memcmp(this, &a, sizeof(Superblock)) == 0;
    }
    inline bool operator!=(const Superblock & a) const
    {
        return !(*this == a);
    }
    
    //compare relevant superblock fields and print differences
    void diff(const Superblock & a, std::string indent = "  ", std::ostream & out = std::cout);

};

struct Block_group_descriptor
{
    uint32_t bg_block_bitmap;
    uint32_t bg_inode_bitmap;
    uint32_t bg_inode_table;
    uint16_t bg_free_blocks_count;
    uint16_t bg_free_inodes_count;
    uint16_t bg_used_dirs_count;
    uint16_t bg_pad;
    uint8_t bg_reserved[12];
};

struct Inode
{
    uint16_t i_mode;
    uint16_t i_uid;
    uint32_t i_size;
    int32_t i_atime;
    int32_t i_ctime;
    int32_t i_mtime;
    int32_t i_dtime;
    uint16_t i_gid;
    uint16_t i_links_count;
    uint32_t i_blocks;
    uint32_t i_flags;
    uint32_t i_osd1;
    union
    {
        uint32_t ptr[15];
        uint8_t symlink[60];
    } i_block;
    uint32_t i_generation;
    uint32_t i_file_acl;
    uint32_t i_dir_acl;
    uint32_t i_faddr;
    uint8_t i_osd2[12];

    //byte-for-byte comparisons
    inline bool operator==(const Inode & a) const
    {
        return memcmp(this, &a, sizeof(Inode)) == 0;
    }
    inline bool operator!=(const Inode & a) const
    {
        return !(*this == a);
    }
    
    //quickly evaluate useful filetypes
    inline bool is_short_symlink() const
    {
        return ((i_mode >> 12) & 0xF) == 0xA && i_size < 60;
    }
    inline bool is_dir() const
    {
        return ((i_mode >> 12) & 0xF) == 0x4;
    }

    //ext2 rev 1+ splits the file size between 2 32bit fields (i_size for low bits and i_dir_acl for high bits)
    //the prior version always set i_dir_acl to 0
    //combine the 2 fields to get file size
    inline uint64_t size() const
    {
        return ((uint64_t)i_size & 0xFFFFFFFF) | ((uint64_t)i_dir_acl << 32);
    }
    
    //compare inode fields and print differences
    void diff(const Inode & a, std::string indent = "  ", std::ostream & out = std::cout);
};

struct Directory_entry
{
    uint32_t inode;
    uint16_t rec_len;
    uint8_t name_len;
    uint8_t file_type;
    std::string name;
    //is the entry a directory?
    inline bool is_dir() const
    {
        return file_type == 2;
    }

};

//main ext2 fs class
class Ext2
{
public:
    //constructor - open the image file and read the core fs data strcutures
    Ext2(const std::string & Filename);
    
    //read in superblock, block group descriptor table, get fs setup
    //The Ext2 object is not properly initialized until this method succeeds.
    void read_sblock();
    
    //read an inode from disk
    //inode table for block group requested is cached
    Inode itab(const int i);
    
    //get an array of blocks pointed to by an inode
    std::vector<int> get_data_blocks(const Inode & i);

    //read a directory and store entries in a useful format
    std::vector<Directory_entry> get_dir_list(const std::vector<int> & data_blocks);
    std::vector<Directory_entry> get_dir_list(const int data_block);

    //lookup a file using relative path. Returns inode # if found, 0 otherwise
    int file_lookup_rel(const std::string & path, const std::vector<Directory_entry> & dir);
    //lookup a file using absolute path. Returns inode # if found, 0 otherwise
    int file_lookup_abs(const std::string & path);

    //traverse a directory, print filenames
    void ls(const std::vector<Directory_entry> & dir_list, bool verbose = false);

    //recursively traverse a directory, print filenames
    void ls_recurse(const std::vector<Directory_entry> & dir_list, const std::string & path = "./", 
        bool verbose = false, int max_depth = 10);

    //read block data into a buffer
    uint8_t * read_block(size_t block_no, uint8_t * buffer);
    
    std::string filename;
    std::ifstream image;
    Superblock sblock;
    std::vector<Block_group_descriptor> bgdtab;
    unsigned int itab_cache_group;
    std::vector<uint8_t> itab_cache;

    //calculated values
    unsigned int num_groups;
    unsigned int block_size;
};
#endif
