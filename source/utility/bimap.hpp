#ifndef BITMAP_BIMAP_HPP_INCLUDED
#define BITMAP_BIMAP_HPP_INCLUDED
#include <string>
#include <vector>
#include <bitset>
#include <cmath>

#include "common/global/global_defines.hpp"

namespace bmap
{
    namespace
    {
        template<unsigned int base>
        std::vector<int> to_base(const std::size_t& num)
        {
            std::size_t tempnum(num);
            std::vector<int> new_num;
            
            while(tempnum > 0)
            {
                new_num.insert(new_num.begin(), (tempnum % base));
                tempnum /= base;
            }
            return new_num;
        }
        
        template<unsigned int base>
        std::size_t from_base(const std::vector<int>& num)
        {
            std::size_t new_num(0);
            for(unsigned long x{num.size() - 1}; true; --x)
            {
                new_num += (num[x] * std::pow(base, (num.size() - (1 + x))));
                if(x == 0) break;
            }
            return new_num;
        }
        
        
    }
    
    static constexpr std::size_t uchar_bitsize{sizeof(unsigned char) * 8};
    
    namespace
    {
        template<std::size_t size> std::bitset<(size * uchar_bitsize)> char_to_bin(const std::string&);
        template<std::size_t size> std::string bin_to_char(const std::bitset<size>&);
        template<std::size_t size> std::vector<int> bitset_to_vec(const std::bitset<size>&);
        template<std::size_t size> std::bitset<size> vec_to_bitset(const std::vector<int>&);
        
        
        template<std::size_t size>
        inline std::bitset<(size * uchar_bitsize)> char_to_bin(const std::string& ch)
        {
            std::bitset<(size * uchar_bitsize)> binary_rep;
            std::vector<int> tempnum;
            binary_rep.reset();
            
            for(unsigned int x{0}; x < size; ++x) tempnum.push_back((int)ch[x]);
            tempnum = std::move(to_base<2>(from_base<256>(tempnum)));
            
            /* If this happens, somthing is terribly terribly wrong... */
            if(tempnum.size() > (size * uchar_bitsize)) ethrow("Vector produced \
by character -> binary conversion was to large for bitset!");
            for(unsigned long x{((size * uchar_bitsize) - tempnum.size())}; x < tempnum.size(); ++x)
            {
                if(tempnum[x] > 1) ethrow("Binary number tainted: conversion failed!");
                binary_rep[x] = tempnum[x];
            }
            return binary_rep;
        }
        
        template<std::size_t size>
        inline std::string bin_to_char(const std::bitset<size>& b)
        {
            std::string ch((size / uchar_bitsize), 0x00);
            std::vector<int> tempnum(std::move(to_base<256>(from_base<2>(bitset_to_vec(b)))));
            
            if(tempnum.size() > (size / uchar_bitsize)) ethrow("Vector produced \
is larger than the specified bitset!");
            for(unsigned long x{((size / uchar_bitsize) - tempnum.size())}; x < tempnum.size(); ++x)
            {
                ch[x] = (char)tempnum[x];
            }
            return ch;
        }
        
        template<std::size_t size>
        inline std::vector<int> bitset_to_vec(const std::bitset<size>& b)
        {
            std::vector<int> v;
            for(unsigned int x{0}; x < b.size(); ++x) v.emplace_back(std::move(b[x]));
            return v;
        }
        
        template<std::size_t size>
        inline std::bitset<size> vec_to_bitset(const std::vector<int>& v)
        {
            std::bitset<size> b;
            for(unsigned int x{0}; x < size; ++x) b[x] = v[x];
            return b;
        }
        
        
    }
    
    
}

#endif