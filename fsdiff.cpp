//fsdiff.cpp
//Matthew Chandler
//CS 680 Project
//11-1-11

//find differences between two filesystems
//only ext2 support is currently implemented
//Architecture notes: We assume little-endianness to simplify reading of ext2 data structures
//this will not work on big-endian systems. 

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
//we're using the new unordered_map container for speed. g++ has supported it for a while with the -std=c++0x flag
//msvc 2010 also supports it by default
//If unordered_map is not availible, simply find and replace 'unordered_map' with 'map', and the program will compile, though slightly slower
#include <unordered_map>
#include <set>

#include "ext2.h"


//recursively traverse a directory, store filenames into a hash table, and a common set
void traverse_fs(Ext2 & fs, const std::vector<Directory_entry> & dir_list, const std::string & path,
    std::unordered_map<std::string, Inode> & files, std::set<std::string> & files_union)
{
    for(size_t i =0; i < dir_list.size(); i++)
    {
        //skip . and .. to avoid infinite loops
        if(dir_list[i].name == "." || dir_list[i].name == "..")
            continue; 
        //store the filename and inode
        files[path + dir_list[i].name] = fs.itab(dir_list[i].inode);
        files_union.insert(path + dir_list[i].name);
        if(dir_list[i].is_dir())
            traverse_fs(fs, fs.get_dir_list(fs.get_data_blocks(fs.itab(dir_list[i].inode))), path + dir_list[i].name + "/", files, files_union);
    }
}

int main(int argc, char ** argv)
{
    //get command line arguments
    if(argc < 3)
    {
        std::cerr<<"Usage: fsdiff image1 image2"<<std::endl;
        return 1;
    }
    std::string image1_name = argv[1], image2_name = argv[2];

    //save some effort
    if(image1_name == image2_name)
        return 0;

    try
    {
        //each image has a hash table of filename - inode pairs to greatly speed up lookups after the initial pass
        std::unordered_map<std::string, Inode> files1, files2;
        //we store the union of filenames from each image into here.
        std::set<std::string> files_union;
        
        //read in fs data, and traverse the fs, storing all the filenames
        std::cerr<<"Reading "<<image1_name<<"...";
        Ext2 fs1(image1_name);
        //add the root directory to the hash table and union
        files1["/"] = fs1.itab(fs1.file_lookup_abs("/"));
        files_union.insert("/");
        //get the root directory
        std::vector<Directory_entry> dir1 = fs1.get_dir_list(fs1.get_data_blocks(fs1.itab(fs1.file_lookup_abs("/"))));
        traverse_fs(fs1, dir1, "/", files1, files_union);
        std::cerr<<" done"<<std::endl;

        std::cerr<<"Reading "<<image2_name<<"...";
        Ext2 fs2(image2_name);
        //add the root directory to the hash table (/ is already in the unio)n
        files2["/"] = fs2.itab(fs2.file_lookup_abs("/"));
        //get the root directory
        std::vector<Directory_entry> dir2 = fs2.get_dir_list(fs2.get_data_blocks(fs2.itab(fs2.file_lookup_abs("/"))));
        traverse_fs(fs2, dir2, "/", files2, files_union);
        std::cerr<<" done"<<std::endl;

        //check for superblock differences
        if(fs1.sblock != fs2.sblock)
        {
            std::cout<<"Superblocks do not match"<<std::endl;
            //print sblock diffs
            fs1.sblock.diff(fs2.sblock);
        }

        //traverse the union of the filesystems
        for(std::set<std::string>::iterator i = files_union.begin(); i != files_union.end(); i++)
        {
            //file only in 1
            if(files1.count(*i) && !files2.count(*i))
                std::cout<<"< "<<*i<<std::endl;
            //file only in 2
            if(!files1.count(*i) && files2.count(*i))
                std::cout<<"> "<<*i<<std::endl;
            //file in 1 and 2
            if(files1.count(*i) && files2.count(*i))
            {
                std::ostringstream msg;
                bool print_msg = false;
                msg<<*i<<std::endl;
                //compare inodes
                if(files1[*i] !=  files2[*i])
                {
                    //print msg, print inode diffs
                    files1[*i].diff(files2[*i],"  ", msg);
                    print_msg = true;
                }
                //get the list of blocks used by this file from each fs
                std::vector<int> blocks1 = fs1.get_data_blocks(files1[*i]);
                std::vector<int> blocks2 = fs2.get_data_blocks(files2[*i]);
                //compare data location
                if(blocks1 != blocks2)
                {
                    //print msg
                    msg<<"  data locations do not match"<<std::endl;
                    print_msg = true;
                }

                //compare file data
                if(files1[*i].size() != files2[*i].size())
                {
                    msg<<"  file data does not match"<<std::endl;
                    print_msg = true;
                }
                else if(blocks1.size() != 0 && blocks2.size() != 0)//don't try on an empty file!
                {
                    //do a byte-for-byte compare
                    //we do not assume that block sizes are equal
                    //buffers to hold 1 block of file data
                    std::vector<uint8_t> buffer1(fs1.block_size), buffer2(fs2.block_size);
                    //indexes into the buffers
                    size_t ptr1 = 0, ptr2 =0;
                    //indexes into the list of blocks
                    size_t blockptr1 = 0, blockptr2 = 0;
                    //bytes read (file sizes are the same, so we only need one of these)
                    size_t bytes_read =0;//use std::streampos
                    //read the first block
                    fs1.read_block(blocks1[blockptr1], &buffer1[0]);
                    fs2.read_block(blocks2[blockptr2], &buffer2[0]);
                    //read through the file.
                    while(bytes_read < files1[*i].size())
                    {
                        //check to see if we need to read a new block
                        if(ptr1 >= buffer1.size())
                        {
                            ptr1 = 0;
                            fs1.read_block(blocks1[blockptr1++], &buffer1[0]);
                        }
                        if(ptr2 >= buffer2.size())
                        {
                            ptr2 = 0;
                            fs2.read_block(blocks2[blockptr2++], &buffer2[0]);
                        }
                        //compare bytes
                        if(buffer1[ptr1++] != buffer2[ptr2++])
                        {
                            msg<<"  file data does not match"<<std::endl;
                            print_msg = true;
                            break;
                        }
                        bytes_read ++;
                    }
                }
                //print our finfings
                if(print_msg)
                    std::cout<<msg.str();
            }
        }
    }
    //stop on any exception (all *should* print an error message to cerr)
    catch(...)
    {
        return 1;
    }

    return 0;
}

