/* Copyright (c) 2006, NIF File Format Library and Tools
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above
     copyright notice, this list of conditions and the following
     disclaimer in the documentation and/or other materials provided
     with the distribution.

   * Neither the name of the NIF File Format Library and Tools
     project nor the names of its contributors may be used to endorse
     or promote products derived from this software without specific
     prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE. */



#include "stdafx.h"
#include "DAOFormat.h"
#include "vector2f.h"
#include "vector3f.h"
#include "vector4f.h"
#include "color4.h"
#include "quaternion.h"
#include "matrix33.h"
#include "matrix43.h"
#include "matrix44.h"

const Matrix44 Matrix44::IDENTITY( 1.0f, 0.0f, 0.0f, 0.0f,
								  0.0f, 1.0f, 0.0f, 0.0f,
								  0.0f, 0.0f, 1.0f, 0.0f,
								  0.0f, 0.0f, 0.0f, 1.0f );

Matrix44::Matrix44() {
	*this = Matrix44::IDENTITY;
}

Matrix44::Matrix44( const Matrix33 & r ) {
	//Set this matrix with rotate and translate information
	Matrix44 & m = *this;
	m[0][0] = r[0][0];  m[0][1] = r[0][1];  m[0][2] = r[0][2];  m[0][3] = 0.0f;
	m[1][0] = r[1][0];  m[1][1] = r[1][1];  m[1][2] = r[1][2];  m[1][3] = 0.0f;
	m[2][0] = r[2][0];  m[2][1] = r[2][1];  m[2][2] = r[2][2];  m[2][3] = 0.0f;
	m[3][0] = 0.0f;     m[3][1] = 0.0f;     m[3][2] = 0.0f;     m[3][3] = 1.0f;
}

Matrix44::Matrix44( const Vector3f & t, const Matrix33 & r, float scale ) {
	//Set up a matrix with rotate and translate information
	Matrix44 rt;
	rt[0][0] = r[0][0];	rt[0][1] = r[0][1];	rt[0][2] = r[0][2];	rt[0][3] = 0.0f;
	rt[1][0] = r[1][0];	rt[1][1] = r[1][1];	rt[1][2] = r[1][2];	rt[1][3] = 0.0f;
	rt[2][0] = r[2][0];	rt[2][1] = r[2][1];	rt[2][2] = r[2][2];	rt[2][3] = 0.0f;
	rt[3][0] = t.x;     rt[3][1] = t.y;     rt[3][2] = t.z;     rt[3][3] = 1.0f;

	//Set up another matrix with the scale information
	Matrix44 s;
	s[0][0] = scale;	s[0][1] = 0.0f;		s[0][2] = 0.0f;		s[0][3] = 0.0f;
	s[1][0] = 0.0f;		s[1][1] = scale;	s[1][2] = 0.0f;		s[1][3] = 0.0f;
	s[2][0] = 0.0f;		s[2][1] = 0.0f;		s[2][2] = scale;	s[2][3] = 0.0f;
	s[3][0] = 0.0f;		s[3][1] = 0.0f;		s[3][2] = 0.0f;		s[3][3] = 1.0f;

	//Multiply the two for the combined transform
	*this = s * rt;
}

Matrix44::Matrix44(  float m11, float m12, float m13, float m14
				   , float m21, float m22, float m23, float m24
				   , float m31, float m32, float m33, float m34
				   , float m41, float m42, float m43, float m44 )
{
	m[0][0] = m11; m[0][1] = m12; m[0][2] = m13; m[0][3] = m14;
	m[1][0] = m21; m[1][1] = m22; m[1][2] = m23; m[1][3] = m24;
	m[2][0] = m31; m[2][1] = m32; m[2][2] = m33; m[2][3] = m34;
	m[3][0] = m41; m[3][1] = m42; m[3][2] = m43; m[3][3] = m44;
}

Matrix44::Matrix44( const Vector3f& pos, const Quaternion& rot, float scale )
{
	Set(pos, rot, scale);
}

Matrix44::Matrix44( const Vector3f& pos, const Quaternion& rot, const Vector3f& scale )
{
	Set(pos, rot, scale);
}

Matrix44::Matrix44( const Vector4f& pos, const Quaternion& rot, float scale )
{
	Set(pos, rot, scale);
}

Matrix44::Matrix44( const Vector4f& pos, const Quaternion& rot, const Vector3f& scale )
{
	Set(pos, rot, scale);
}

Matrix33 Matrix44::GetRotation() const {
	const Matrix44 & t = *this;

	Matrix33 m( t[0][0], t[0][1], t[0][2],
		t[1][0], t[1][1], t[1][2],
		t[2][0], t[2][1], t[2][2]
	);

	//--Extract Scale from first 3 a--//
	float scale[3];
	for (int r = 0; r < 3; ++r) {
		//Get scale for this row
		scale[r] = Vector3f( m[r][0], m[r][1], m[r][2] ).Length();

		//Normalize the row by dividing each factor by scale
		m[r][0] /= scale[r];
		m[r][1] /= scale[r];
		m[r][2] /= scale[r];
	}

	//Return result
	return m;
}

float Matrix44::GetScale() const {
	const Matrix44 & m = *this;
	float scale[3];
	for (int r = 0; r < 3; ++r) {
		//Get scale for this row
		scale[r] = Vector3f( m[r][0], m[r][1], m[r][2] ).Length();
	}

	//averate the scale since NIF doesn't support discreet scaling
	return (scale[0] + scale[1] + scale[2]) / 3.0f;
}


Vector3f Matrix44::GetTranslation() const {
	const Matrix44 & m = *this;
	return Vector3f( m[3][0], m[3][1], m[3][2] );
}

Matrix44 Matrix44::operator*( const Matrix44 & rh ) const {
	return Matrix44(*this) *= rh;
}
Matrix44 & Matrix44::operator*=( const Matrix44 & rh ) {
	Matrix44 r;
	Matrix44 & lh = *this;
	float t;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			t = 0.0f;
			for (int k = 0; k < 4; k++) {
				t += lh[i][k] * rh[k][j];
			}
			r[i][j] = t;
		}
	}

	*this = r;
	return *this;
}

Matrix44 Matrix44::operator*( float rh ) const {
	return Matrix44(*this) *= rh;
}

Matrix44 & Matrix44::operator*=( float rh ) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			(*this)[i][j] *= rh;
		}
	}
	return *this;
}

Vector3f Matrix44::operator*( const Vector3f & rh ) const {
	const Matrix44 & t = *this;
	Vector3f v;
	//Multiply, ignoring w
	v.x = rh.x * t[0][0] + rh.y * t[1][0] + rh.z * t[2][0] + t[3][0];
	v.y = rh.x * t[0][1] + rh.y * t[1][1] + rh.z * t[2][1] + t[3][1];
	v.z = rh.x * t[0][2] + rh.y * t[1][2] + rh.z * t[2][2] + t[3][2];
	//answer[3] = rh[0] * t(0,3) + rh[1] * t(1,3) + rh[2] * t(2,3) + t(3,3);

	return v;
}

Matrix44 Matrix44::operator+( const Matrix44 & rh ) const {
	return Matrix44(*this) += rh;
} 

Matrix44 & Matrix44::operator+=( const Matrix44 & rh ) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			(*this)[i][j] += rh[i][j];
		}
	}
	return *this;
}

Matrix44 Matrix44::operator-( const Matrix44 & rh ) const {
	return Matrix44(*this) -= rh;
} 

Matrix44 & Matrix44::operator-=( const Matrix44 & rh ) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			(*this)[i][j] -= rh[i][j];
		}
	}
	return *this;
}

Matrix44 & Matrix44::operator=( const Matrix44 & rh ) {
	memcpy(m, rh.m, sizeof(float) * 4 * 4);
	return *this;
}

bool Matrix44::operator==( const Matrix44 & rh ) const {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if ( (*this)[i][j] != rh[i][j] )
				return false;
		}
	}
	return true;
}

bool Matrix44::operator!=( const Matrix44 & rh ) const {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if ( (*this)[i][j] != rh[i][j] )
				return true;
		}
	}
	return false;
}

Matrix44 Matrix44::Transpose() const {
	const Matrix44 & t = *this;
	return Matrix44( t[0][0], t[1][0], t[2][0], t[3][0],
		t[0][1], t[1][1], t[2][1], t[3][1],
		t[0][2], t[1][2], t[2][2], t[3][2],
		t[0][3], t[1][3], t[2][3], t[3][3] );
}

Matrix33 Matrix44::Submatrix( int skip_r, int skip_c ) const {
	Matrix33 sub;
	int i = 0, j = 0;
	for (int r = 0; r < 4; r++) {
		if (r == skip_r)
			continue;
		for (int c = 0; c < 4; c++) {
			if (c == skip_c)
				continue;
			sub[i][j] = (*this)[r][c];
			j++;
		}
		i++;
		j = 0;
	}
	return sub;
}

float Matrix44::Adjoint( int skip_r, int skip_c ) const {
	Matrix33 sub = Submatrix( skip_r, skip_c );
	return pow(-1.0f, float(skip_r + skip_c)) * sub.Determinant();
}

Matrix44 Matrix44::Inverse() const {
	Matrix44 result;

	float det = Determinant();
	for (int r = 0; r < 4; r++) {
		for (int c = 0; c < 4; c++) {
			result[c][r] = Adjoint(r, c) / det;
		}
	}

	return result;
}

float Matrix44::Determinant() const {
	const Matrix44 & t = *this;
	return  t[0][0] * Submatrix(0, 0).Determinant()
		- t[0][1] * Submatrix(0, 1).Determinant()
		+ t[0][2] * Submatrix(0, 2).Determinant()
		- t[0][3] * Submatrix(0, 3).Determinant();
}

void Matrix44::Decompose( Vector3f & translate, Matrix33 & rotation, Vector3f & scale ) const
{
	translate = Vector3f( (*this)[3][0], (*this)[3][1], (*this)[3][2] );
	Matrix33 rotT;
	for ( int i = 0; i < 3; i++ ){
		for ( int j = 0; j < 3; j++ ){
			rotation[i][j] = (*this)[i][j];
			rotT[j][i] = (*this)[i][j];
		}
	}
	Matrix33 mtx = rotation * rotT;
	scale.Set( sqrt(mtx[0][0]), sqrt(mtx[1][1]), sqrt(mtx[2][2]) );
	for ( int i = 0; i < 3; i++ ){
		for ( int j = 0; j < 3; j++ ){
			rotation[i][j] /= scale[i];
		}
	}
}

void Matrix44::Decompose( Vector3f & translate, Matrix33 & rotation, float & scale ) const {
	Vector3f scale3;
	Decompose(translate, rotation, scale3);
	scale = (scale3[0] + scale3[1] + scale3[2]) / 3.0f;
}

void Matrix44::Decompose( Vector3f & translate, Quaternion & rotation, float & scale ) const {
	Matrix33 m3;
	Vector3f scale3;
	Decompose(translate, m3, scale3);
	rotation = m3.AsQuaternion();
	scale = (scale3[0] + scale3[1] + scale3[2]) / 3.0f;
}
void Matrix44::Decompose( Vector3f & translate, Quaternion & rotation, Vector3f & scale ) const {
	Matrix33 m3;
	Decompose(translate, m3, scale);
	rotation = m3.AsQuaternion();
}


Matrix44& Matrix44::Set(const Vector3f& pos, const Quaternion& rot, float scale)
{
	Matrix44 rt;
	rot.MakeMatrix(rt);
	rt.SetRow(3, pos);
	if (scale == 1.0)
	{
		*this = rt;
	}
	else
	{
		Matrix44 s(true);
		s[0][0] = s[1][1] = s[2][2] = s[3][3] = 1.0;
		*this = s * rt;
	}
	return *this;
}

Matrix44& Matrix44::Set(const Vector3f& pos, const Quaternion& rot, const Vector3f& scale)
{
	Matrix44 rt;
	rot.MakeMatrix(rt);
	rt.SetRow(3, pos);
	if (scale[0] == 1.0 && scale[1] == 1.0 && scale[2] == 1.0)
	{
		*this = rt;
	}
	else
	{
		Matrix44 s(true);
		s[0][0] = scale.x;
		s[1][1] = scale.y;
		s[2][2] = scale.z;
		*this = s * rt;
	}
	return *this;
}


Matrix44& Matrix44::Set(const Vector4f& pos, const Quaternion& rot, float scale)
{
	Matrix44 rt;
	rot.MakeMatrix(rt);
	rt.SetRow(3, pos);
	if (scale == 1.0)
	{
		*this = rt;
	}
	else
	{
		Matrix44 s(true);
		s[0][0] = s[1][1] = s[2][2] = 1.0;
		*this = s * rt;
	}
	return *this;
}

Matrix44& Matrix44::Set(const Vector4f& pos, const Quaternion& rot, const Vector3f& scale)
{
	Matrix44 rt;
	rot.MakeMatrix(rt);
	rt.SetRow(3, pos);
	if (scale[0] == 1.0 && scale[1] == 1.0 && scale[2] == 1.0)
	{
		*this = rt;
	}
	else
	{
		Matrix44 s(true);
		s[0][0] = scale.x;
		s[1][1] = scale.y;
		s[2][2] = scale.z;
		*this = s * rt;
	}
	return *this;
}

Quaternion Matrix44::GetRot() const
{
	Quaternion quat;
	float tr, s, q[4];
	int i, j, k;

	int nxt[3] = {1, 2, 0};

	// compute the trace of the matrix
	tr = m[0][0] + m[1][1] + m[2][2];

	// check if the trace is positive or negative
	if (tr > 0.0) {
		s = sqrt (tr + 1.0f);
		quat.w = s / 2.0f;
		s = 0.5f / s;
		quat.x = (m[1][2] - m[2][1]) * s;
		quat.y = (m[2][0] - m[0][2]) * s;
		quat.z = (m[0][1] - m[1][0]) * s;
	}
	else {
		// trace is negative
		i = 0;
		if ( m[1][1] > m[0][0])
			i = 1;
		if ( m[2][2] > m[i][i] )
			i = 2;
		j = nxt[i];
		k = nxt[j];

		s = sqrt( ( m[i][i] - (m[j][j] + m[k][k]) ) + 1.0f );
		q[i] = s * 0.5f;
		if (s != 0.0f) s = 0.5f / s;
		q[3] = (m[j][k] - m[k][j]) * s;
		q[j] = (m[i][j] + m[j][i]) * s;
		q[k] = (m[i][k] + m[k][i]) * s;
		quat.x = q[0];
		quat.y= q[1];
		quat.z = q[2];
		quat.w = q[3];
	}

	return quat;
}

