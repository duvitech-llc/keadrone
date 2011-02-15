/*
 * vector.h
 *
 *  Created on: 15 feb 2011
 *      Author: Willem (wnpd.nl)
 */

#ifndef VECTOR_H_
#define VECTOR_H_

float Vector_Dot_Product(float *vector1, float *vector2);
void Vector_Cross_Product(float *vectorOut, float *v1, float *v2);
void Vector_Scale(float *vectorOut, float *vectorIn, float scale2);
void Vector_Add(float *vectorOut, float *vectorIn1, float *vectorIn2);

#endif /* VECTOR_H_ */
