/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _Z1_H_RPCGEN
#define _Z1_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif

#define BAJT 255

struct osoba {
	char *imie;
	char *nazwisko;
	int rok_urodzenia;
};
typedef struct osoba osoba;

typedef quad_t duza_liczba;

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_osoba (XDR *, osoba*);
extern  bool_t xdr_duza_liczba (XDR *, duza_liczba*);

#else /* K&R C */
extern bool_t xdr_osoba ();
extern bool_t xdr_duza_liczba ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_Z1_H_RPCGEN */
