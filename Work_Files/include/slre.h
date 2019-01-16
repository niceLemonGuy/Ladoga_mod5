/*
 * Copyright (c) 2004-2013 Sergey Lyubka <valenok@gmail.com>
 * Copyright (c) 2013 Cesanta Software Limited
 * All rights reserved
 *
 * This library is dual-licensed: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. For the terms of this
 * license, see <http://www.gnu.org/licenses/>.
 *
 * You are free to use this library under the terms of the GNU General
 * Public License, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * Alternatively, you can license this library under a commercial
 * license, as set out in <http://cesanta.com/products.html>.
 */

/*
 * This is a regular expression library that implements a subset of Perl RE.
 * Please refer to README.md for a detailed reference.


(?i)    Must be at the beginning of the regex. Makes match case-insensitive
^       Match beginning of a buffer
$       Match end of a buffer
()      Grouping and substring capturing
\s      Match whitespace
\S      Match non-whitespace
\d      Match decimal digit
\n      Match new line character
\r      Match line feed character
\f      Match form feed character
\v      Match vertical tab character
\t      Match horizontal tab character
\b      Match backspace character
+       Match one or more times (greedy)
+?      Match one or more times (non-greedy)
*       Match zero or more times (greedy)
*?      Match zero or more times (non-greedy)
?       Match zero or once (non-greedy)
x|y     Match x or y (alternation operator)
\meta   Match one of the meta character: ^$().[]*+?|\
\xHH    Match byte with hex value 0xHH, e.g. \x4a
[...]   Match any character from set. Ranges like [a-z] are supported
[^...]  Match any character but ones from set

Example: parsing HTTP request line
const char *request = " GET /index.html HTTP/1.0\r\n\r\n";
struct slre_cap caps[4];

if (slre_match("^\\s*(\\S+)\\s+(\\S+)\\s+HTTP/(\\d)\\.(\\d)",
               request, strlen(request), caps, 4, 0) > 0) {
  printf("Method: [%.*s], URI: [%.*s]\n",
         caps[0].len, caps[0].ptr,
         caps[1].len, caps[1].ptr);
} else {
  printf("Error parsing [%s]\n", request);
}

 */

#ifndef SLRE_HEADER_DEFINED
#define SLRE_HEADER_DEFINED

#ifdef __cplusplus
extern "C" {
#endif

struct slre_cap {
  const char *ptr;
  int len;
};


int slre_match(const char *regexp, const char *buf, int buf_len,
               struct slre_cap *caps, int num_caps, int flags);

/* Possible flags for slre_match() */
enum { SLRE_IGNORE_CASE = 1 };


/* slre_match() failure codes */
#define SLRE_NO_MATCH               -1
#define SLRE_UNEXPECTED_QUANTIFIER  -2
#define SLRE_UNBALANCED_BRACKETS    -3
#define SLRE_INTERNAL_ERROR         -4
#define SLRE_INVALID_CHARACTER_SET  -5
#define SLRE_INVALID_METACHARACTER  -6
#define SLRE_CAPS_ARRAY_TOO_SMALL   -7
#define SLRE_TOO_MANY_BRANCHES      -8
#define SLRE_TOO_MANY_BRACKETS      -9
#define SLRE_RECURSION_ERROR				-10

#define SLRE_RECURSION_LEVEL				5
#ifdef __cplusplus
}
#endif

#endif  /* SLRE_HEADER_DEFINED */
