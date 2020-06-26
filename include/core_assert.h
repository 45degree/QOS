#ifndef ASSERT_H
#define ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif

/* magic chars used by `printx' */
#define MAG_CH_ASSERT	'\003'

#define core_assert(exp) if(exp); \
    else assert_failure(#exp, __FILE__, __BASE_FILE__, __LINE__);

/**
 * @brief 
 * 
 * @param exp 
 * @param file 
 * @param basefile 
 * @param line 
 */
void assert_failure(char* exp, char* file, char* basefile, int line);

#ifdef __cplusplus
};
#endif

#endif
