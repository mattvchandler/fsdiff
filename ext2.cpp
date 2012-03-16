//ext2.cpp
//Copyright 2012 Matthew Chandler
//CS 680 Project
//11-1-11

//methods for the various ext2 structures and fs class
//ext2 information from http://www.nongnu.org/ext2-doc/ext2.html

#include <iomanip>
#include <exception>
#include <cstring>
#include <cmath>
#include <ctime>

#include "ext2.h"

//get a nicely formatted string from a unix timestamp
std::string decode_timestamp(const int32_t timestamp)
{
    time_t time = timestamp;
    std::string timestr = asctime(gmtime(&time));
    //trim trailing newline
    return timestr.substr(0,timestr.size()-1);
}

//compare relevant superblock fields and print differences
void Superblock::diff(const Superblock & a, std::string indent, std::ostream & out)
{
    if(s_inodes_count != a.s_inodes_count)
        out<<indent<<"inode count:\n"<<indent<<"< "<<s_inodes_count<<"\n"<<indent<<"> "<<a.s_inodes_count<<std::endl;
    if(s_blocks_count != a.s_blocks_count)
        out<<indent<<"block count:\n"<<indent<<"< "<<s_blocks_count<<"\n"<<indent<<"> "<<a.s_blocks_count<<std::endl;
    if(s_r_blocks_count != a.s_r_blocks_count)
        out<<indent<<"reserved block count:\n"<<indent<<"< "<<s_r_blocks_count<<"\n"<<indent<<"> "<<a.s_r_blocks_count<<std::endl;
    if(s_free_blocks_count != a.s_free_blocks_count)
        out<<indent<<"free block count:\n"<<indent<<"< "<<s_free_blocks_count<<"\n"<<indent<<"> "<<a.s_free_blocks_count<<std::endl;
    if(s_free_inodes_count != a.s_free_inodes_count)
        out<<indent<<"free inode count:\n"<<indent<<"< "<<s_free_inodes_count<<"\n"<<indent<<"> "<<a.s_free_inodes_count<<std::endl;
    if(s_first_data_block != a.s_first_data_block)
        out<<indent<<"first data block:\n"<<indent<<"< "<<s_first_data_block<<"\n"<<indent<<"> "<<a.s_first_data_block<<std::endl;
    if(s_log_block_size != a.s_log_block_size)//show diff in block_size instead of log
        out<<indent<<"block size:\n"<<indent<<"< "<<(1024 << s_log_block_size)<<"\n"<<indent<<"> "<<(1024 << a.s_log_block_size)<<std::endl;
    if(s_log_frag_size != a.s_log_frag_size)
        out<<indent<<"fragment size:\n"<<indent<<"< "<<(1024 << s_log_frag_size)<<"\n"<<indent<<"> "<<(1024 << a.s_log_frag_size)<<std::endl;
    if(s_blocks_per_group != a.s_blocks_per_group)
        out<<indent<<"blocks per group:\n"<<indent<<"< "<<s_blocks_per_group<<"\n"<<indent<<"> "<<a.s_blocks_per_group<<std::endl;
    if(s_frags_per_group != a.s_frags_per_group)
        out<<indent<<"frags per group:\n"<<indent<<"< "<<s_frags_per_group<<"\n"<<indent<<"> "<<a.s_frags_per_group<<std::endl;
    if(s_inodes_per_group != a.s_inodes_per_group)
        out<<indent<<"inodes per group:\n"<<indent<<"< "<<s_inodes_per_group<<"\n"<<indent<<"> "<<a.s_inodes_per_group<<std::endl;
    if(s_mtime != a.s_mtime)
        out<<indent<<"last mount time (UTC):\n"<<indent<<"< "<<decode_timestamp(s_mtime)<<"\n"<<indent<<"> "<<decode_timestamp(a.s_mtime)<<std::endl;
    if(s_wtime != a.s_wtime)
        out<<indent<<"last write time (UTC):\n"<<indent<<"< "<<decode_timestamp(s_wtime)<<"\n"<<indent<<"> "<<decode_timestamp(a.s_wtime)<<std::endl;
    if(s_mnt_count != a.s_mnt_count)
        out<<indent<<"mount count:\n"<<indent<<"< "<<s_mnt_count<<"\n"<<indent<<"> "<<a.s_mnt_count<<std::endl;
    if(s_max_mnt_count != a.s_max_mnt_count)
        out<<indent<<"max mount count:\n"<<indent<<"< "<<s_max_mnt_count<<"\n"<<indent<<"> "<<a.s_max_mnt_count<<std::endl;
    if(s_state != a.s_state)
        out<<indent<<"clean unmount:\n"<<indent<<"< "<<(s_state==1?"yes":"no")<<"\n"<<indent<<"> "<<(a.s_state==1?"yes":"no")<<std::endl;
    if(s_errors != a.s_errors)
    {
        out<<indent<<"error handling:\n"<<indent<<"< ";
        switch(s_errors)
        {
        case 1:
            out<<"continue\n"<<indent<<"> ";
            break;
        case 2:
            out<<"remount read-only\n"<<indent<<"> ";
            break;
        case 3:
            out<<"kernel panic\n"<<indent<<"> ";
            break;
        default:
            out<<"unknown\n"<<indent<<"> ";
            break;
        }
        switch(a.s_state)
        {
        case 1:
            out<<"continue"<<std::endl;
            break;
        case 2:
            out<<"remount read-only"<<std::endl;
            break;
        case 3:
            out<<"kernel panic"<<std::endl;
            break;
        default:
            out<<"unknown"<<std::endl;
            break;
        }
    }
    if(s_minor_rev_level != a.s_minor_rev_level)
        out<<indent<<"minor revison level:\n"<<indent<<"< "<<s_minor_rev_level<<"\n"<<indent<<"> "<<a.s_minor_rev_level<<std::endl;
    if(s_lastcheck != a.s_lastcheck)
        out<<indent<<"last error check (UTC):\n"<<indent<<"< "<<decode_timestamp(s_inodes_count)<<"\n"<<indent<<"> "<<decode_timestamp(a.s_inodes_count)<<std::endl;
    if(s_checkinterval != a.s_checkinterval)
        out<<indent<<"error check interval (in days):\n"<<indent<<"< "<<s_inodes_count/86400.0f<<"\n"<<indent<<"> "<<a.s_inodes_count/86400.0f<<std::endl;
    if(s_creator_os != a.s_creator_os)
    {
        out<<indent<<"creator os:\n"<<indent<<"< ";
        switch(s_creator_os)
        {
        case 0:
            out<<"Linux\n"<<indent<<"> ";
            break;
        case 1:
            out<<"GNU Hurd\n"<<indent<<"> ";
            break;
        case 2:
            out<<"MASIX\n"<<indent<<"> ";
            break;
        case 3:
            out<<"FreeBSD\n"<<indent<<"> ";
            break;
        case 4:
            out<<"Lites\n"<<indent<<"> ";
            break;
        default:
            out<<"Unknown\n"<<indent<<"> ";
            break;
        }
        switch(a.s_creator_os)
        {
        case -1:
            out<<"Linux"<<std::endl;
            break;
        case 1:
            out<<"GNU Hurd"<<std::endl;
            break;
        case 2:
            out<<"MASIX"<<std::endl;
            break;
        case 3:
            out<<"FreeBSD"<<std::endl;
            break;
        case 4:
            out<<"Lites"<<std::endl;
            break;
        default:
            out<<"Unknown"<<std::endl;
            break;
        }
    }
    if(s_rev_level != a.s_rev_level)
        out<<indent<<"revision level:\n"<<indent<<"< "<<s_rev_level<<"\n"<<indent<<"> "<<a.s_rev_level<<std::endl;
    if(s_def_resuid != a.s_def_resuid)
        out<<indent<<"user id for reserved blocks:\n"<<indent<<"< "<<s_def_resuid<<"\n"<<indent<<"> "<<a.s_def_resuid<<std::endl;
    if(s_def_resgid != a.s_def_resgid)
        out<<indent<<"group id for reserved blocks:\n"<<indent<<"< "<<s_def_resgid<<"\n"<<indent<<"> "<<a.s_def_resgid<<std::endl;
    if(s_first_ino != a.s_first_ino)
        out<<indent<<"first inode:\n"<<indent<<"< "<<s_first_ino<<"\n"<<indent<<"> "<<a.s_first_ino<<std::endl;
    if(s_inode_size != a.s_inode_size)
        out<<indent<<"inode size:\n"<<indent<<"< "<<s_inode_size<<"\n"<<indent<<"> "<<a.s_inode_size<<std::endl;
    if(s_feature_compat != a.s_feature_compat)
        out<<indent<<"compatible feature bitmask:\n"<<indent<<"< "<<s_feature_compat<<"\n"<<indent<<"> "<<a.s_feature_compat<<std::endl;
    if(s_feature_incompat != a.s_feature_incompat)
        out<<indent<<"incompatible feature bitmask:\n"<<indent<<"< "<<s_feature_incompat<<"\n"<<indent<<"> "<<a.s_feature_incompat<<std::endl;
    if(s_feature_ro_compat != a.s_feature_ro_compat)
        out<<indent<<"read-only compatible feature bitmask:\n"<<indent<<"< "<<s_feature_ro_compat<<"\n"<<indent<<"> "<<a.s_feature_ro_compat<<std::endl;
    if(strncmp((char *)s_uuid, (char *)a.s_uuid, 16) != 0)
    {
        out<<"uuid:\n"<<indent<<"< ";
        size_t c = 0;
        while(c<16) out<<std::setfill('0')<<std::setw(2)<<std::hex<<(int)s_uuid[c++];
        out<<"\n"<<indent<<"> ";
        c = 0;
        while(c<16) out<<std::setfill('0')<<std::setw(2)<<std::hex<<(int)a.s_uuid[c++];
        out<<std::dec<<std::endl;
    }
    if(strncmp((char *)s_volume_name, (char *)a.s_volume_name, 16) != 0)
        out<<indent<<"volume name:\n"<<indent<<"< "<<s_volume_name<<"\n"<<indent<<"> "<<a.s_volume_name<<std::endl;
    if(strncmp((char *)s_last_mounted, (char *)a.s_last_mounted, 64) != 0)
        out<<indent<<"last mounted at:\n"<<indent<<"< "<<s_last_mounted<<"\n"<<indent<<"> "<<a.s_last_mounted<<std::endl;
    if(s_algo_bitmap != a.s_algo_bitmap)
        out<<indent<<"compression algorithm code:\n"<<indent<<"< "<<s_algo_bitmap<<"\n"<<indent<<"> "<<a.s_algo_bitmap<<std::endl;
}

//compare inode fields and print differences
void Inode::diff(const Inode & a, std::string indent, std::ostream & out)
{
    //mode is tricky. It stores file type, permissions, and more in a single bitmask
    //we serperate these out and compare them seperately if the whole field does not match
    if(i_mode != a.i_mode)
    {
        int f_type1 = (i_mode >> 12) & 0xF;
        int f_type2 = (a.i_mode >> 12) & 0xF;
        if(f_type1 != f_type2)
        {
            out<<indent<<"file type:\n"<<indent<<"< ";
            switch(f_type1)
            {
            case 0xC:
                out<<"socket\n"<<indent<<"> ";
                break;
            case 0xA:
                out<<"symlink\n"<<indent<<"> ";
                break;
            case 0x8:
                out<<"regular file\n"<<indent<<"> ";
                break;
            case 0x6:
                out<<"block device\n"<<indent<<"> ";
                break;
            case 0x4:
                out<<"directory\n"<<indent<<"> ";
                break;
            case 0x2:
                out<<"character device\n"<<indent<<"> ";
                break;
            case 0x1:
                out<<"fifo\n"<<indent<<"> ";
                break;
            }
            switch(f_type2)
            {
            case 0xC:
                out<<"socket"<<std::endl;
                break;
            case 0xA:
                out<<"symlink"<<std::endl;
                break;
            case 0x8:
                out<<"regular file"<<std::endl;
                break;
            case 0x6:
                out<<"block device"<<std::endl;
                break;
            case 0x4:
                out<<"directory"<<std::endl;
                break;
            case 0x2:
                out<<"character device"<<std::endl;
                break;
            case 0x1:
                out<<"fifo"<<std::endl;
                break;
            }
        }
        if(((i_mode << 11) & 0x1) != ((a.i_mode << 11) & 0x1))
        {
            out<<indent<<"set process user id\n"<<indent<<"< ";
            out<<(((i_mode>>11) & 0x1)?"y":"n");
            out<<"\n"<<indent<<"> ";
            out<<(((a.i_mode>>11) & 0x1)?"y":"n");
            out<<std::endl;
        }
        if(((i_mode << 10) & 0x1) != ((a.i_mode << 10) & 0x1))
        {
            out<<indent<<"set process group id\n"<<indent<<"< ";
            out<<(((i_mode>>10) & 0x1)?"y":"n");
            out<<"\n"<<indent<<"> ";
            out<<(((a.i_mode>>10) & 0x1)?"y":"n");
            out<<std::endl;
        }
        if(((i_mode << 11) & 0x1) != ((a.i_mode << 11) & 0x1))
        {
            out<<indent<<"sticky bit\n"<<indent<<"< ";
            out<<(((i_mode>>9) & 0x1)?"y":"n");
            out<<"\n"<<indent<<"> ";
            out<<(((a.i_mode>>9) & 0x1)?"y":"n");
            out<<std::endl;
        }
        int perm1 = i_mode & 0x1FF;
        int perm2 = a.i_mode & 0x1FF;
        if(perm1 != perm2)
        {
            out<<indent<<"permissions:\n"<<indent<<"< ";
            out<<(((perm1>>8) & 0x1)?"r":"-");
            out<<(((perm1>>7) & 0x1)?"w":"-");
            out<<(((perm1>>6) & 0x1)?"x":"-");
            out<<(((perm1>>5) & 0x1)?"r":"-");
            out<<(((perm1>>4) & 0x1)?"w":"-");
            out<<(((perm1>>3) & 0x1)?"x":"-");
            out<<(((perm1>>2) & 0x1)?"r":"-");
            out<<(((perm1>>1) & 0x1)?"w":"-");
            out<<((perm1 & 0x1)?"x":"-");
            out<<"\n"<<indent<<"> ";
            out<<(((perm2>>8) & 0x1)?"r":"-");
            out<<(((perm2>>7) & 0x1)?"w":"-");
            out<<(((perm2>>6) & 0x1)?"x":"-");
            out<<(((perm2>>5) & 0x1)?"r":"-");
            out<<(((perm2>>4) & 0x1)?"w":"-");
            out<<(((perm2>>3) & 0x1)?"x":"-");
            out<<(((perm2>>2) & 0x1)?"r":"-");
            out<<(((perm2>>1) & 0x1)?"w":"-");
            out<<((perm2 & 0x1)?"x":"-");
            out<<std::endl;
        }
    }
    if(i_uid != a.i_uid)
        out<<indent<<"user id:\n"<<indent<<"< "<<i_uid<<"\n"<<indent<<"> "<<a.i_uid<<std::endl;
    if(size() != a.size())
        out<<indent<<"size:\n"<<indent<<"< "<<size()<<"\n"<<indent<<"> "<<a.size()<<std::endl;
    if(i_atime != a.i_atime)
        out<<indent<<"access time (UTC):\n"<<indent<<"< "<<decode_timestamp(i_atime)<<"\n"<<indent<<"> "<<decode_timestamp(a.i_atime)<<std::endl;
    if(i_ctime != a.i_ctime)
        out<<indent<<"creation time (UTC):\n"<<indent<<"< "<<decode_timestamp(i_ctime)<<"\n"<<indent<<"> "<<decode_timestamp(a.i_ctime)<<std::endl;
    if(i_mtime != a.i_mtime)
        out<<indent<<"modification time (UTC):\n"<<indent<<"< "<<decode_timestamp(i_mtime)<<"\n"<<indent<<"> "<<decode_timestamp(a.i_mtime)<<std::endl;
    if(i_dtime != a.i_dtime)
        out<<indent<<"deletion time (UTC):\n"<<indent<<"< "<<decode_timestamp(i_dtime)<<"\n"<<indent<<"> "<<decode_timestamp(a.i_dtime)<<std::endl;
    if(i_gid != a.i_gid)
        out<<indent<<"group id:\n"<<indent<<"< "<<i_gid<<"\n"<<indent<<"> "<<a.i_gid<<std::endl;
    if(i_links_count != a.i_links_count)
        out<<indent<<"# hard links:\n"<<indent<<"< "<<i_links_count<<"\n"<<indent<<"> "<<a.i_links_count<<std::endl;
    if(i_blocks != a.i_blocks)
        out<<indent<<"# 512-byte blocks allocated:\n"<<indent<<"< "<<i_blocks<<"\n"<<indent<<"> "<<a.i_blocks<<std::endl;
    if(i_osd1 != a.i_osd1)
        out<<indent<<"1st OS dependent structure changed"<<std::endl;
    if(i_flags != a.i_flags)
        out<<indent<<"flag bitmask:\n"<<indent<<"< "<<i_flags<<"\n"<<indent<<"> "<<a.i_flags<<std::endl;
    if(is_short_symlink() && a.is_short_symlink() && strncmp((char *)i_block.symlink, (char*)a.i_block.symlink, 60) !=0)
        out<<indent<<"symlink path:\n"<<indent<<"< "<<i_block.symlink<<"\n"<<indent<<"> "<<a.i_block.symlink<<std::endl;
    if(i_generation != a.i_generation)
        out<<indent<<"generation:\n"<<indent<<"< "<<i_generation<<"\n"<<indent<<"> "<<a.i_generation<<std::endl;
    if(i_file_acl != a.i_file_acl)
        out<<indent<<"extended attribute block #:\n"<<indent<<"< "<<i_file_acl<<"\n"<<indent<<"> "<<a.i_file_acl<<std::endl;
    if(i_faddr != a.i_faddr)
        out<<indent<<"i_faddr:\n"<<indent<<"< "<<i_faddr<<"\n"<<indent<<"> "<<a.i_faddr<<std::endl;
    if(memcmp(i_osd2, a.i_osd2, 12) != 0)
        out<<indent<<"2nd OS dependent structure changed"<<std::endl;
}


//constructor - open the image file and read the core fs data strcutures
Ext2::Ext2(const std::string & Filename):filename(Filename)
{
    image.open(filename.c_str(), std::ios::binary);
    if(!image)
    {
        std::cerr<<"error opening "<<filename<<" for reading."<<std::endl;
        throw std::ios_base::failure("unable to open file");
    }
    read_sblock();
}

//read in superblock, block group descriptor table, get fs setup
//The Ext2 object is not properly initialized until this method succeeds.
void Ext2::read_sblock()
{
    //read superblock
    image.seekg(1024);
    image.read((char *)&sblock,1024);
    if(!image)
    {
        std::cerr<<"Error reading superblock from "<<filename<<std::endl;
        throw std::ios_base::failure("Error reading superblock");
    }
    
    //check the magic number
    //odds are good that nothing else is going to work if this is wrong
    if(sblock.s_magic != 0xEF53)
    {
        std::cerr<<"Error, superblock magic number is wrong in "<<filename<<"\nImage may not be a valid ext2 fs"<<std::endl;
        throw std::ios_base::failure("bad superblock magic number");
    }

    //do some sanity checking
    if(sblock.s_blocks_count <=0 || sblock.s_blocks_per_group <=0 || sblock.s_inodes_count <=0 || sblock.s_inodes_per_group <=0
        || sblock.s_inode_size <=0)
    {
        std::cerr<<"Bad superblock data in "<<filename<<std::endl;
        throw std::ios_base::failure("Bad superblock data");
    }

    //calculate actual block size
    block_size = 1024 << sblock.s_log_block_size;
    num_groups = (unsigned int)ceil(sblock.s_blocks_count / (float)sblock.s_blocks_per_group);

    //read block group descriptor table
    bgdtab.resize(num_groups);
    image.seekg((std::streampos)(sblock.s_first_data_block + 1) * (std::streampos)block_size);
    image.read((char *)&bgdtab.at(0),sizeof(Block_group_descriptor)*num_groups);
    if(!image)
    {
        std::cerr<<"Error reading Block Group discriptor table from "<<filename<<std::endl;
        throw std::ios_base::failure("Error reading Block Group discriptor table");
    }

    //I don't need to read the block and inode bitmaps, as they are more for writing to the fs than for reading

    //setup the inode table cache
    itab_cache_group =-1;
    itab_cache.clear();
    itab_cache.resize(sblock.s_inodes_per_group * sblock.s_inode_size);
}

//read an inode from disk
//inode table for block group requested is cached
Inode Ext2::itab(const int i)
{
    //In ext2 revision 1+, inodes can have variable sizes (power of 2, <= block size)
    //from what I can tell, the first 128 bytes is the same regardless of size, so I'll just waste the remainder
    //What we do is read the inode table as bytes, find the correct offset, and cast to an Inode
    unsigned int group = (i - 1) / sblock.s_inodes_per_group;
    if(group != itab_cache_group)
    {
        try
        {
            itab_cache_group = group;
            image.seekg((std::streampos)bgdtab.at(group).bg_inode_table * (std::streampos)block_size);
            image.read((char *)&itab_cache[0], sblock.s_inode_size * sblock.s_inodes_per_group);
            if(!image)
                throw std::ios_base::failure("Error reading inode table");
        }
        catch(...)
        {
            std::cerr<<"Error reading inode table on inode # "<<i<<" from "<<filename<<"\nTried to read "
                <<sblock.s_inode_size * sblock.s_inodes_per_group<<" bytes at offset "
                <<(std::streampos)bgdtab.at(group).bg_inode_table * (std::streampos)block_size<<std::endl;
            itab_cache_group = -1;
            throw;
        }
    }
    //using pointer conversion to cast uint8_t to Inode
    return *((Inode *)&itab_cache[((i-1) % sblock.s_inodes_per_group) * sblock.s_inode_size]);
}

//get an array of blocks pointed to by an inode
std::vector<int> Ext2::get_data_blocks(const Inode & i)
{
    std::vector<int> blocks;
    //check to see if this is a short symlink, which stores a string rather than block pointers
    if(i.is_short_symlink())
        return blocks; //return an empty list if it i

    //how many blocks are there?
    unsigned int num_blocks = i.size() / (uint64_t)block_size + ((i.size() % (uint64_t)block_size == 0)?0:1);

    //get normal blocks
    for(size_t j=0; j<12; j++)
    {
        blocks.push_back(i.i_block.ptr[j]);
        if(blocks.size() == num_blocks)
            return blocks;
    }
    //get from indirect blocks (if any)
    if(i.i_block.ptr[12] != 0)
    {
        std::vector<int> indirect(block_size/4);
        image.seekg((std::streampos)i.i_block.ptr[12] * (std::streampos)block_size);
        image.read((char *)&indirect[0], block_size);
        if(!image)
        {
            std::cerr<<"Error reading block#"<<i.i_block.ptr[12] * block_size<<" from "<<filename<<"\nTried to read "
                <<block_size<<" bytes."<<std::endl;
            throw std::ios_base::failure("Error reading image in get_data_blocks (indirect)");
        }
        for(size_t j=0; j<indirect.size(); j++)
        {
            blocks.push_back(indirect[j]);
            if(blocks.size() == num_blocks)
                return blocks;
        }
    }
    //get from doubly indirect blocks (if any)
    if(i.i_block.ptr[13] != 0)
    {
        std::vector<int> doubly_indirect(block_size/4);
        image.seekg((std::streampos)i.i_block.ptr[13] * (std::streampos)block_size);
        image.read((char *)&doubly_indirect[0], block_size);
        if(!image)
        {
            std::cerr<<"Error reading block#"<<i.i_block.ptr[13] * block_size<<" from "<<filename<<"\nTried to read "
                <<block_size<<" bytes."<<std::endl;
            throw std::ios_base::failure("Error reading image in get_data_blocks (2x indirect#1)");
        }
        for(size_t j=0; j<doubly_indirect.size(); j++)
        {
            if(doubly_indirect[j] != 0)
            {
                std::vector<int> indirect(block_size/4);
                image.seekg((std::streampos)doubly_indirect[j] * (std::streampos)block_size);
                image.read((char *)&indirect[0], block_size);
                if(!image)
                {
                    std::cerr<<"Error reading block#"<<doubly_indirect[j] * block_size<<" from "
                        <<filename<<"\nTried to read "<<block_size<<" bytes."<<std::endl;
                    throw std::ios_base::failure("Error reading image in get_data_blocks (2x indirect#2)");
                }
                for(size_t k=0; k<indirect.size(); k++)
                {
                    blocks.push_back(indirect[k]);
                    if(blocks.size() == num_blocks)
                        return blocks;
                }
            }
        }
    }
    //get from triply indirect blocks (if any)
    if(i.i_block.ptr[14] != 0)
    {
        std::vector<int> triply_indirect(block_size/4);
        image.seekg((std::streampos)i.i_block.ptr[14] * (std::streampos)block_size);
        image.read((char *)&triply_indirect[0], block_size);
        if(!image)
        {
            std::cerr<<"Error reading block#"<<i.i_block.ptr[14] * block_size<<" from "<<filename<<"\nTried to read "
                <<block_size<<" bytes."<<std::endl;
            throw std::ios_base::failure("Error reading image in get_data_blocks (3x indirect#1)");
        }
        for(size_t j=0; j<triply_indirect.size(); j++)
        {
            if(triply_indirect[j] != 0)
            {
                std::vector<int> doubly_indirect(block_size/4);
                image.seekg((std::streampos)triply_indirect[j] * (std::streampos)block_size);
                image.read((char *)&doubly_indirect[0], block_size);
                if(!image)
                {
                    std::cerr<<"Error reading block#"<<triply_indirect[j] * block_size<<" from "
                        <<filename<<"\nTried to read "<<block_size<<" bytes."<<std::endl;
                    throw std::ios_base::failure("Error reading image in get_data_blocks (3x indirect#2)");
                }
                for(size_t k=0; k<doubly_indirect.size(); k++)
                {
                    if(doubly_indirect[k] != 0)
                    {
                        std::vector<int> indirect(block_size/4);
                        image.seekg((std::streampos)doubly_indirect[k] * (std::streampos)block_size);
                        image.read((char *)&indirect[0], block_size);
                        if(!image)
                        {
                            std::cerr<<"Error reading block#"<<doubly_indirect[k] * block_size<<" from "
                                <<filename<<"\nTried to read "<<block_size<<" bytes."<<std::endl;
                            throw std::ios_base::failure("Error reading image in get_data_blocks (3x indirect#3)");
                        }
                        for(size_t l=0; l<indirect.size(); l++)
                        {
                            blocks.push_back(indirect[l]);
                            if(blocks.size() == num_blocks)
                                return blocks;
                        }
                    }
                }
            }
        }
    }
    return blocks;
}

//read a directory and store entries in a useful format
std::vector<Directory_entry> Ext2::get_dir_list(const std::vector<int> & data_blocks)
{
    //read in all of the data
    std::vector<uint8_t> buffer;
    buffer.resize(data_blocks.size() * block_size);
    for(size_t i=0; i<data_blocks.size(); i++)
    {
        image.seekg((std::streampos)data_blocks[i] * (std::streampos)block_size);
        image.read((char *)&buffer[i*block_size], block_size);
        if(!image)
        {
            std::cerr<<"Error reading block# "<<data_blocks[i]<<" from "<<filename<<"\nTried to read "<<block_size<<" bytes."<<std::endl;
            throw std::ios_base::failure("Error reading image in get_dir_list");
        }
    }
    std::vector<Directory_entry> dir_list;
    size_t ptr=0;
    while(ptr < buffer.size())
    {
        Directory_entry ent;
        memcpy(&ent, &buffer[ptr], 8);
        ent.name.assign((char *)&buffer[ptr+8], ent.name_len);
        if(ent.inode != 0) dir_list.push_back(ent);
        ptr+=ent.rec_len;
    }
    return dir_list;
}
std::vector<Directory_entry> Ext2::get_dir_list(const int data_block)
{
    std::vector<int> data_blocks;
    data_blocks.push_back(data_block);
    return get_dir_list(data_blocks);
}

//lookup a file using relative path. Returns inode # if found, 0 otherwise
int Ext2::file_lookup_rel(const std::string & path, const std::vector<Directory_entry> & in_dir)
{
    std::vector<Directory_entry> dir = in_dir;
    size_t strptr = 0;
    size_t dirptr=0;
    while(path[strptr] == '/')
        strptr++;
    while(true)
    {   
        if(dirptr >= dir.size())
            return 0;//file not found
        Directory_entry i = dir[dirptr];
        if(i.name == path.substr(strptr, i.name.size()))
        {
            strptr += i.name.size();
            while(path[strptr] == '/')
                strptr++;
            if(strptr >= path.size())
                return i.inode;
            dir = get_dir_list(get_data_blocks(itab(i.inode)));
            dirptr =0;
        }
        else
            dirptr++;
    }
}
//lookup a file using absolute path. Returns inode # if found, 0 otherwise
int Ext2::file_lookup_abs(const std::string & path)
{
    //fs root is always at inode 2 in ext2
    if(path == "/")
        return 2;
    else
        return  file_lookup_rel(path, get_dir_list(get_data_blocks(itab(2))));
}


//traverse a directory, print filenames
void Ext2::ls(const std::vector<Directory_entry> & dir_list, bool verbose)
{
    for(size_t i=0; i < dir_list.size(); i++)
    {
        if(dir_list[i].name == "." || dir_list[i].name == "..")
            continue; 
        std::cout<<dir_list[i].name;
        if(verbose)
        {
            std::cout<<" inode: "<<dir_list[i].inode<<std::endl;
            Inode inode = itab(dir_list[i].inode);
            std::cout<<"size: "<<inode.size()<<" num blocks : "
                <<get_data_blocks(inode).size()<<std::endl;
            if(inode.is_short_symlink())
                std::cout<<"short symlink: "<<inode.i_block.symlink<<std::endl;
        }
        std::cout<<std::endl;
    }
}

//recursively traverse a directory, print filenames
void Ext2::ls_recurse(const std::vector<Directory_entry> & dir_list, const std::string & path, 
    bool verbose, int max_depth )
{
    if(max_depth <=0)
        return;
    for(size_t i=0; i < dir_list.size(); i++)
    {
        if(dir_list[i].name == "." || dir_list[i].name == "..")
            continue; 
        std::cout<<path<<dir_list[i].name;
        if(verbose)
        {
            std::cout<<" inode: "<<dir_list[i].inode<<std::endl;
            Inode inode = itab(dir_list[i].inode);
            std::cout<<"size: "<<inode.size()<<" num blocks : "
                <<get_data_blocks(inode).size()<<std::endl;
            if(inode.is_short_symlink())
                std::cout<<"short symlink: "<<inode.i_block.symlink<<std::endl;
        }
        std::cout<<std::endl;
        if(dir_list[i].is_dir() && dir_list[i].name != "." && dir_list[i].name != "..")
            ls_recurse(get_dir_list(get_data_blocks(itab(dir_list[i].inode))), path + dir_list[i].name + "/",
                verbose, max_depth -1);
    }
}

//read block data into a buffer
uint8_t * Ext2::read_block(size_t block_no, uint8_t * buffer)
{
    //return a block of zeros if the block_no is 0 to support sparse files
    if(block_no == 0)
    {
        memset(buffer, '\0', block_size);
        return buffer;
    }
    if(block_no > sblock.s_blocks_count)
    {
        std::cerr<<"Block # "<<block_no<<" out of bounds"<<std::endl;
        throw std::ios_base::failure("Error reading image in read_block");
    }
    image.seekg((std::streampos)block_no * (std::streampos)block_size);
    image.read((char *)buffer, block_size);
    if(!image)
    {
        std::cerr<<"Error reading block# "<<block_no<<" from "<<filename<<"\nTried to read "<<block_size<<" bytes."<<std::endl;
        throw std::ios_base::failure("Error reading image in read_block");
    }
    return buffer;
}
