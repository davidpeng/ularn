/* =============================================================================
 * PROGRAM:  ularn
 * FILENAME: saveutils.c
 *
 * DESCRIPTION:
 * This module contains utilities used in loading and saving games.
 *
 * =============================================================================
 * EXPORTED VARIABLES
 *
 * FileSum : The current checksum for the file being written/read.
 *
 * =============================================================================
 * EXPORTED FUNCTIONS
 *
 * sum    : Checksum calculation function
 * bwrite : Binary write with checksum update
 * bread  : Binary read with checksum update.
 *
 * =============================================================================
 */

#include <stdio.h>
#include <errno.h>

#include "ularn_win.h"
#include "scores.h"
#include "saveutils.h"

/* =============================================================================
 * Local variables
 */

 /* The number of characters written */
static int w = 0;

/* The number of characters read */
static int r = 0;

/* =============================================================================
 * Exported variables
 */

int FileSum;

/* =============================================================================
 * Exported functions
 */

/* =============================================================================
 * FUNCTION: sum
 */
unsigned int sum(unsigned char *data, int n)
{
  unsigned int sum;
  int c, nb;

  sum = nb = 0;
  while (nb++ < n)
  {
    c = *data++;
    if (sum & 01)
      sum = (sum >> 1) + 0x8000;
    else
      sum >>= 1;
    sum += c;
    sum &= 0xFFFF;
  }
  return(sum);
}

/* =============================================================================
 * FUNCTION: bwrite
 */
void bwrite(MemoryFile *fp, char *buf, long num)
{
  static int ncalls = 0;

  ncalls++;
  MemoryFile_write(fp, (unsigned char *)buf, num);

  w += num;

  FileSum += sum((unsigned char *)buf, num);
}

/* =============================================================================
 * FUNCTION: bread
 */
void bread(MemoryFile *fp, char *buf, long num)
{
  static int ncalls = 0;

  ncalls++;
  MemoryFile_read(fp, (unsigned char *)buf, num);

  r += num;

  FileSum += sum((unsigned char *)buf, num);
}

