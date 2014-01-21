/*************************************************************************\

  Copyright 1999 The University of North Carolina at Chapel Hill.
  All Rights Reserved.

  Permission to use, copy, modify and distribute this software and its
  documentation for educational, research and non-profit purposes, without
  fee, and without a written agreement is hereby granted, provided that the
  above copyright notice and the following three paragraphs appear in all
  copies.

  IN NO EVENT SHALL THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL BE
  LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
  CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE
  USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY
  OF NORTH CAROLINA HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH
  DAMAGES.

  THE UNIVERSITY OF NORTH CAROLINA SPECIFICALLY DISCLAIM ANY
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE
  PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
  NORTH CAROLINA HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT,
  UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

  The authors may be contacted via:

  US Mail:             E. Larsen
                       Department of Computer Science
                       Sitterson Hall, CB #3175
                       University of N. Carolina
                       Chapel Hill, NC 27599-3175

  Phone:               (919)962-1749

  EMail:               geom@cs.unc.edu


\**************************************************************************/

//--------------------------------------------------------------------------
// File:   TriDist.cpp
// Author: Eric Larsen
// Description:
// contains SegPoints() for finding closest points on a pair of line
// segments and TriDist() for finding closest points on a pair of triangles
//--------------------------------------------------------------------------

#include "MatVec.h"
#include "TriDist.h"
#ifdef _WIN32
#include <float.h>
#define isnan _isnan
#endif

namespace PQP
{

//--------------------------------------------------------------------------
// SegPoints() 
//
// Returns closest points between an segment pair.
// Implemented from an algorithm described in
//
// Vladimir J. Lumelsky,
// On fast computation of distance between line segments.
// In Information Processing Letters, no. 21, pages 55-61, 1985.   
//--------------------------------------------------------------------------

void
Tri_Processor::SegPoints(PQP_REAL VEC[3], 
	  PQP_REAL X[3], PQP_REAL Y[3],             // closest points
          const PQP_REAL P[3], const PQP_REAL A[3], // seg 1 origin, vector
          const PQP_REAL Q[3], const PQP_REAL B[3]) // seg 2 origin, vector
{
  PQP_REAL T[3], A_dot_A, B_dot_B, A_dot_B, A_dot_T, B_dot_T;
  PQP_REAL TMP[3];

  pqp_math.VmV(T,Q,P);
  A_dot_A = pqp_math.VdotV(A,A);
  B_dot_B = pqp_math.VdotV(B,B);
  A_dot_B = pqp_math.VdotV(A,B);
  A_dot_T = pqp_math.VdotV(A,T);
  B_dot_T = pqp_math.VdotV(B,T);

  // t parameterizes ray P,A 
  // u parameterizes ray Q,B 

  PQP_REAL t,u;

  // compute t for the closest point on ray P,A to
  // ray Q,B

  PQP_REAL denom = A_dot_A*B_dot_B - A_dot_B*A_dot_B;

  t = (A_dot_T*B_dot_B - B_dot_T*A_dot_B) / denom;

  // clamp result so t is on the segment P,A

  if ((t < 0) || isnan(t)) t = 0; else if (t > 1) t = 1;

  // find u for point on ray Q,B closest to point at t

  u = (t*A_dot_B - B_dot_T) / B_dot_B;

  // if u is on segment Q,B, t and u correspond to 
  // closest points, otherwise, clamp u, recompute and
  // clamp t 

  if ((u <= 0) || isnan(u)) {

    pqp_math.VcV(Y, Q);

    t = A_dot_T / A_dot_A;

    if ((t <= 0) || isnan(t)) {
      pqp_math.VcV(X, P);
      pqp_math.VmV(VEC, Q, P);
    }
    else if (t >= 1) {
      pqp_math.VpV(X, P, A);
      pqp_math.VmV(VEC, Q, X);
    }
    else {
      pqp_math.VpVxS(X, P, A, t);
      pqp_math.VcrossV(TMP, T, A);
      pqp_math.VcrossV(VEC, A, TMP);
    }
  }
  else if (u >= 1) {

    pqp_math.VpV(Y, Q, B);

    t = (A_dot_B + A_dot_T) / A_dot_A;

    if ((t <= 0) || isnan(t)) {
      pqp_math.VcV(X, P);
      pqp_math.VmV(VEC, Y, P);
    }
    else if (t >= 1) {
      pqp_math.VpV(X, P, A);
      pqp_math.VmV(VEC, Y, X);
    }
    else {
      pqp_math.VpVxS(X, P, A, t);
      pqp_math.VmV(T, Y, P);
      pqp_math.VcrossV(TMP, T, A);
      pqp_math.VcrossV(VEC, A, TMP);
    }
  }
  else {

    pqp_math.VpVxS(Y, Q, B, u);

    if ((t <= 0) || isnan(t)) {
      pqp_math.VcV(X, P);
      pqp_math.VcrossV(TMP, T, B);
      pqp_math.VcrossV(VEC, B, TMP);
    }
    else if (t >= 1) {
      pqp_math.VpV(X, P, A);
      pqp_math.VmV(T, Q, X);
      pqp_math.VcrossV(TMP, T, B);
      pqp_math.VcrossV(VEC, B, TMP);
    }
    else {
      pqp_math.VpVxS(X, P, A, t);
      pqp_math.VcrossV(VEC, A, B);
      if (pqp_math.VdotV(VEC, T) < 0) {
        pqp_math.VxS(VEC, VEC, -1);
      }
    }
  }
}

//--------------------------------------------------------------------------
// TriDist() 
//
// Computes the closest points on two triangles, and returns the 
// distance between them.
// 
// S and T are the triangles, stored tri[point][dimension].
//
// If the triangles are disjoint, P and Q give the closest points of 
// S and T respectively. However, if the triangles overlap, P and Q 
// are basically a random pair of points from the triangles, not 
// coincident points on the intersection of the triangles, as might 
// be expected.
//--------------------------------------------------------------------------

PQP_REAL 
Tri_Processor::TriDist(PQP_REAL P[3], PQP_REAL Q[3],
        const PQP_REAL S[3][3], const PQP_REAL T[3][3])  
{
  // Compute vectors along the 6 sides

  PQP_REAL Sv[3][3], Tv[3][3];
  PQP_REAL VEC[3];

  pqp_math.VmV(Sv[0],S[1],S[0]);
  pqp_math.VmV(Sv[1],S[2],S[1]);
  pqp_math.VmV(Sv[2],S[0],S[2]);

  pqp_math.VmV(Tv[0],T[1],T[0]);
  pqp_math.VmV(Tv[1],T[2],T[1]);
  pqp_math.VmV(Tv[2],T[0],T[2]);

  // For each edge pair, the vector connecting the closest points 
  // of the edges defines a slab (parallel planes at head and tail
  // enclose the slab). If we can show that the off-edge vertex of 
  // each triangle is outside of the slab, then the closest points
  // of the edges are the closest points for the triangles.
  // Even if these tests fail, it may be helpful to know the closest
  // points found, and whether the triangles were shown disjoint

  PQP_REAL V[3];
  PQP_REAL Z[3];
  PQP_REAL minP[3], minQ[3], mindd;
  int shown_disjoint = 0;

  mindd = pqp_math.VdistV2(S[0],T[0]) + 1;  // Set first minimum safely high

  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      // Find closest points on edges i & j, plus the 
      // vector (and distance squared) between these points

      SegPoints(VEC,P,Q,S[i],Sv[i],T[j],Tv[j]);
      
      pqp_math.VmV(V,Q,P);
      PQP_REAL dd = pqp_math.VdotV(V,V);

      // Verify this closest point pair only if the distance 
      // squared is less than the minimum found thus far.

      if (dd <= mindd)
      {
        pqp_math.VcV(minP,P);
        pqp_math.VcV(minQ,Q);
        mindd = dd;

        pqp_math.VmV(Z,S[(i+2)%3],P);
        PQP_REAL a = pqp_math.VdotV(Z,VEC);
        pqp_math.VmV(Z,T[(j+2)%3],Q);
        PQP_REAL b = pqp_math.VdotV(Z,VEC);

        if ((a <= 0) && (b >= 0)) return sqrt(dd);

        PQP_REAL p = pqp_math.VdotV(V, VEC);

        if (a < 0) a = 0;
        if (b > 0) b = 0;
        if ((p - a + b) > 0) shown_disjoint = 1;	
      }
    }
  }

  // No edge pairs contained the closest points.  
  // either:
  // 1. one of the closest points is a vertex, and the
  //    other point is interior to a face.
  // 2. the triangles are overlapping.
  // 3. an edge of one triangle is parallel to the other's face. If
  //    cases 1 and 2 are not true, then the closest points from the 9
  //    edge pairs checks above can be taken as closest points for the
  //    triangles.
  // 4. possibly, the triangles were degenerate.  When the 
  //    triangle points are nearly colinear or coincident, one 
  //    of above tests might fail even though the edges tested
  //    contain the closest points.

  // First check for case 1

  PQP_REAL Sn[3], Snl;       
  pqp_math.VcrossV(Sn,Sv[0],Sv[1]); // Compute normal to S triangle
  Snl = pqp_math.VdotV(Sn,Sn);      // Compute square of length of normal
  
  // If cross product is long enough,

  if (Snl > 1e-15)  
  {
    // Get projection lengths of T points

    PQP_REAL Tp[3]; 

    pqp_math.VmV(V,S[0],T[0]);
    Tp[0] = pqp_math.VdotV(V,Sn);

    pqp_math.VmV(V,S[0],T[1]);
    Tp[1] = pqp_math.VdotV(V,Sn);

    pqp_math.VmV(V,S[0],T[2]);
    Tp[2] = pqp_math.VdotV(V,Sn);

    // If Sn is a separating direction,
    // find point with smallest projection

    int point = -1;
    if ((Tp[0] > 0) && (Tp[1] > 0) && (Tp[2] > 0))
    {
      if (Tp[0] < Tp[1]) point = 0; else point = 1;
      if (Tp[2] < Tp[point]) point = 2;
    }
    else if ((Tp[0] < 0) && (Tp[1] < 0) && (Tp[2] < 0))
    {
      if (Tp[0] > Tp[1]) point = 0; else point = 1;
      if (Tp[2] > Tp[point]) point = 2;
    }

    // If Sn is a separating direction, 

    if (point >= 0) 
    {
      shown_disjoint = 1;

      // Test whether the point found, when projected onto the 
      // other triangle, lies within the face.
    
      pqp_math.VmV(V,T[point],S[0]);
      pqp_math.VcrossV(Z,Sn,Sv[0]);
      if (pqp_math.VdotV(V,Z) > 0)
      {
        pqp_math.VmV(V,T[point],S[1]);
        pqp_math.VcrossV(Z,Sn,Sv[1]);
        if (pqp_math.VdotV(V,Z) > 0)
        {
          pqp_math.VmV(V,T[point],S[2]);
          pqp_math.VcrossV(Z,Sn,Sv[2]);
          if (pqp_math.VdotV(V,Z) > 0)
          {
            // T[point] passed the test - it's a closest point for 
            // the T triangle; the other point is on the face of S

            pqp_math.VpVxS(P,T[point],Sn,Tp[point]/Snl);
            pqp_math.VcV(Q,T[point]);
            return sqrt(pqp_math.VdistV2(P,Q));
          }
        }
      }
    }
  }

  PQP_REAL Tn[3], Tnl;       
  pqp_math.VcrossV(Tn,Tv[0],Tv[1]); 
  Tnl = pqp_math.VdotV(Tn,Tn);      
  
  if (Tnl > 1e-15)  
  {
    PQP_REAL Sp[3]; 

    pqp_math.VmV(V,T[0],S[0]);
    Sp[0] = pqp_math.VdotV(V,Tn);

    pqp_math.VmV(V,T[0],S[1]);
    Sp[1] = pqp_math.VdotV(V,Tn);

    pqp_math.VmV(V,T[0],S[2]);
    Sp[2] = pqp_math.VdotV(V,Tn);

    int point = -1;
    if ((Sp[0] > 0) && (Sp[1] > 0) && (Sp[2] > 0))
    {
      if (Sp[0] < Sp[1]) point = 0; else point = 1;
      if (Sp[2] < Sp[point]) point = 2;
    }
    else if ((Sp[0] < 0) && (Sp[1] < 0) && (Sp[2] < 0))
    {
      if (Sp[0] > Sp[1]) point = 0; else point = 1;
      if (Sp[2] > Sp[point]) point = 2;
    }

    if (point >= 0) 
    { 
      shown_disjoint = 1;

      pqp_math.VmV(V,S[point],T[0]);
      pqp_math.VcrossV(Z,Tn,Tv[0]);
      if (pqp_math.VdotV(V,Z) > 0)
      {
        pqp_math.VmV(V,S[point],T[1]);
        pqp_math.VcrossV(Z,Tn,Tv[1]);
        if (pqp_math.VdotV(V,Z) > 0)
        {
          pqp_math.VmV(V,S[point],T[2]);
          pqp_math.VcrossV(Z,Tn,Tv[2]);
          if (pqp_math.VdotV(V,Z) > 0)
          {
            pqp_math.VcV(P,S[point]);
            pqp_math.VpVxS(Q,S[point],Tn,Sp[point]/Tnl);
            return sqrt(pqp_math.VdistV2(P,Q));
          }
        }
      }
    }
  }

  // Case 1 can't be shown.
  // If one of these tests showed the triangles disjoint,
  // we assume case 3 or 4, otherwise we conclude case 2, 
  // that the triangles overlap.
  
  if (shown_disjoint)
  {
    pqp_math.VcV(P,minP);
    pqp_math.VcV(Q,minQ);
    return sqrt(mindd);
  }
  else return 0;
}

} // namespace
