#ifndef _BASE_H
#define _BASE_H

#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>

#include <random>
#include <iomanip>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

/**
 * @description:  将16进制UUID转换为64进制编码
 * @param {char} *hex_uuid  16进制uuid字符串
 * @param {char} *base64_uuid  转换后的64进制编码
 * @return {*}
 */
void hex_uuid_to_base64(const char *hex_uuid, char *base64_uuid);

/**
 * @description:  将64进制编码的UUID进行还原为16进制UUID
 * @param {char} *base64_uuid 64进制编码的UUID
 * @param {char} *hex_uuid  还原后的16进制UUID
 * @return {*}
 */
void base64_to_hex_uuid(const char *base64_uuid, char *hex_uuid);

#endif