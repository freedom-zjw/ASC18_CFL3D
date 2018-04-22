c  ---------------------------------------------------------------------------
c  CFL3D is a structured-grid, cell-centered, upwind-biased, Reynolds-averaged
c  Navier-Stokes (RANS) code. It can be run in parallel on multiple grid zones
c  with point-matched, patched, overset, or embedded connectivities. Both
c  multigrid and mesh sequencing are available in time-accurate or
c  steady-state modes.
c
c  Copyright 2001 United States Government as represented by the Administrator
c  of the National Aeronautics and Space Administration. All Rights Reserved.
c 
c  The CFL3D platform is licensed under the Apache License, Version 2.0 
c  (the "License"); you may not use this file except in compliance with the 
c  License. You may obtain a copy of the License at 
c  http://www.apache.org/licenses/LICENSE-2.0. 
c 
c  Unless required by applicable law or agreed to in writing, software 
c  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT 
c  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the 
c  License for the specific language governing permissions and limitations 
c  under the License.
c  ---------------------------------------------------------------------------
c
      subroutine delv(jdim,kdim,idim,q,sj,sk,si,vol,ux,wt,blank,iover,
     .                qj0,qk0,qi0,bcj,bck,bci,nbl,volj0,volk0,voli0,
     .                maxbl,vormax,ivmax,jvmax,kvmax)
c
c     $Id$
c
c**********************************************************************
c     Purpose:  Evaluate velocity derivatives at cell centers
c**********************************************************************
c
c      input arrays   : 
c       q             : primitive variables at cell centers
c                       (rho,u,v,w,p)
c       vol           : cell volumes
c       sj,sk,si      : metrics
c                       (direction cosines,areas,speeds of cell faces)
c       ifj,ifk,ifi   : =0 don't compute contribution from that direction
c                       >0 do      "
c      output arrays  :
c       ux            : velocity derivatives
c                           1 = ux    4 = vx    7 = wx
c                           2 = uy    5 = vy    8 = wy
c                           3 = uz    6 = vz    9 = wz
c      scratch arrays :
c       wt(1-3)       : cross-flow temporaries
c
c**********************************************************************
c
#   ifdef CMPLX
      implicit complex(a-h,o-z)
#   endif
c
      dimension q(jdim,kdim,idim,5),ux(jdim-1,kdim-1,idim-1,9)
      dimension sj(jdim,kdim,idim-1,5),sk(jdim,kdim,idim-1,5),
     .          si(jdim,kdim,idim,5)
      dimension vol(jdim,kdim,idim-1),blank(jdim,kdim,idim)
      dimension wt(jdim,kdim,9)
      dimension bcj(kdim,idim-1,2),bck(jdim,idim-1,2),bci(jdim,kdim,2)
      dimension qj0(kdim,idim-1,5,4),
     .          qk0(jdim,idim-1,5,4),qi0(jdim,kdim,5,4)
      dimension volj0(kdim,idim-1,4),
     .          volk0(jdim,idim-1,4),voli0(jdim,kdim,4)
      dimension vormax(maxbl),ivmax(maxbl),jvmax(maxbl),kvmax(maxbl)
c
      common /twod/ i2d
c
c
      ifi=1
      ifj=1
      ifk=1
      jdim1 = jdim-1
      kdim1 = kdim-1
      idim1 = idim-1
c
c
      call c_delv(jdim, kdim, idim, iover, nbl, 
     .            maxbl, i2d, ivmax, jvmax, kvmax, 
     .            q, sj, sk, si, vol, 
     .            ux, wt, blank, qj0, qk0,
     .            qi0, bcj, bck, bci, volj0,
     .            volk0, voli0, vormax)
c

c
c      J-direction contributions is handled by c_delv
c
c
c      K-direction contributions is handled by c_delv
c
c
c      I-direction contributions is handled by c_delv
c
c
c     set velocity derivatives for hole cells to one
c
      onec = 1.
      if (iover.eq.1) then
         do 2403 nk=1,9
         do 2402 i=1,idim1
         do 2402 k=1,kdim1
         do 2402 j=1,jdim1
         ux(j,k,i,nk) = ccvmgt(onec,ux(j,k,i,nk),
     .                  (real(blank(j,k,i)).eq.0.e0))
 2402    continue
 2403    continue
      end if
c
      return
      end
