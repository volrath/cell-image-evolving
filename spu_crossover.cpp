#include <spu_mfcio.h>
#include <stdio.h>

/* here's the local copy of the control block, to be filled by the DMA */
//control_block cb __attribute__ ((aligned (128)));

int main(unsigned long long speid, unsigned long long argp, unsigned long long  envp) 
{

//   /* here is the actual DMA call */
//   /* the first parameter is the address in local store to place the data */
//   /* the second parameter holds the main memory address                  */
//   /* the third parameter holds the number of bytes to DMA                */
//   /* the fourth parameter identifies a "tag" to associate with this DMA  */
//   /* (this should be a number between 0 and 31, inclusive)               */
//   /* the last two parameters are only useful if you've implemented your  */
//   /* own cache replacement management policy.  Otherwise set them to 0.  */

//   mfc_get(&cb, argp, sizeof(cb), 31, 0, 0);

//   /* now, we set the "tag bit" into the correct channel on the hardware  */
//   /* this is always 1 left-shifted by the tag specified with the DMA     */
//   /* for whose completion you wish to wait.                              */

//   mfc_write_tag_mask(1<<31);

//   /* now, issue the read and wait to guarantee DMA completion before we continue. */

//   mfc_read_tag_status_all();

  printf("address received through control \n");

  return 0;
}
