#include <spu_mfcio.h>
#include <stdio.h>

typedef struct {
  float *p1;
  float *p2;
  char dummy[120]; // CREO QUE ES PARA QUE MIDA 128
} pair_parents_t;

int main(unsigned long long speid, unsigned long long argp, unsigned long long  envp) {
  pair_parents_t parents __attribute__((aligned(128)));
  float p1[768] __attribute__((aligned(128)));
  float p2[768] __attribute__((aligned(128)));

  unsigned int spu_id = spu_read_in_mbox(); // De verdad no se si quiero esto;
  int tag = 1, tag_mask = 1 << tag;

  mfc_get(&parents, (unsigned int) argp, envp, tag, 0, 0);
  mfc_write_tag_mask(tag_mask);
  mfc_read_tag_status_all();

  printf("%x\n", parents.p2);
  mfc_get(p1, parents.p1, sizeof(p1), tag, 0, 0);
  mfc_get(p2, parents.p2, sizeof(p2), tag, 0, 0);
  mfc_read_tag_status_all();

  printf("spu #%d parents: %f %f\n", spu_id, p1[0], p2[0]);


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
  return 0;
}
