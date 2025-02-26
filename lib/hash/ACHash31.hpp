/* Copyright (c) 2011, NEC Europe Ltd, Consorzio Nazionale
 * Interuniversitario per le Telecomunicazioni, Institut
 * Telecom/Telecom Bretagne, ETH Zürich, INVEA-TECH a.s. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of NEC Europe Ltd, Consorzio Nazionale
 *      Interuniversitario per le Telecomunicazioni, Institut Telecom/Telecom
 *      Bretagne, ETH Zürich, INVEA-TECH a.s. nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT
 * HOLDERBE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
 */

/**
 * @file    ACHash31.hpp
 * @author  Fabrizio Nuccilli
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This class represents a simple hash: h(i)=(a*i+b)mod p
 * It is optimised for a p = 2^31-1
 * and it works with unsigned int
 */




#ifndef ACHASH31_HPP
#define    ACHASH31_HPP

#define dgstlenhsm 4
#define mod 0x2FFFFFFF

#include "hash.hpp"

namespace blockmon {

class ACHash31 : public HASH
{


public:

    ACHash31(unsigned int a, unsigned int b):
        HASH(dgstlenhsm),SMOD_A(a),SMOD_B(b),SMOD_MOD(mod){}


     /*
     * this method compute the digest of a key
     * @param key is the key that is to be hashed
     * @return the message digest
     */
    unsigned int compute(unsigned int key);

    /*
     * this method compute the digest of a key
     * @param key is the key that is to be hashed
     * @param a is the multiplied int
     * @param b is the added int
     * @param mod is the module
     * @return is the message digest
     */
    unsigned int compute(unsigned int key, unsigned int a, unsigned int b);

    /*
     * this method compute the digest of a key
     * if the key is more then 4 bytes, this method split it in len / 4 pieces
     * then all pieces are added to create the new key which is hashed
     * @param key is the key that is to be hashed
     * @param len is the key length
     * @param output is the array where the message digest is stored
     */
    void compute(const unsigned char key[], int len, unsigned char output[]) ;

    /*
     * this method hashes a file which contains integer
     * it adds all the integer to create a key and then computes the hash function
     * @param filepath the file path
     * @param output is the array where the message digest is stored
     */
    void computeFile(const char *filepath, unsigned char output[]) ;

    /*
     * HMAC
     * this method implements a message authentication code (MAC)
     * involving this hash in combination with a secret key
     * also in this case both the key and the msg are splitted in pieces which max
     * length is 32 bit and then they are added and hashed
     * @param key is the key
     * @param keylen is the key length
     * @param input is the original message
     * @param ilen is the length of the original message
     * @param output is the array where the message digest is stored
     */
    void compute(const unsigned char *key, int keylen, const unsigned char *input, int ilen,
                unsigned char output[]) ;

    /*
     * This method returns the a parameter
     * @return the value of a
     */
    unsigned int get_a();

    /*
     * This method returns the b parameter
     * @return the value of b
     */
    unsigned int get_b();

private:
    unsigned int SMOD_A;
    unsigned int SMOD_B;
    unsigned int SMOD_MOD;
};

}



#endif    /* HASH_SMOD_HPP */
