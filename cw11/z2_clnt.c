/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include <memory.h> /* for memset */
#include "z2.h"

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 25, 0 };

sum_wynik *
sum_oblicz_1(sum_argument *argp, CLIENT *clnt)
{
	static sum_wynik clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, SUM_OBLICZ,
		(xdrproc_t) xdr_sum_argument, (caddr_t) argp,
		(xdrproc_t) xdr_sum_wynik, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}