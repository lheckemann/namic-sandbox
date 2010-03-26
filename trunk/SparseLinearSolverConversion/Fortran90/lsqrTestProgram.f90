!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
! File lsqrTestProgram.f90
!
!    lsqrTestProgram
!
! Main program for testing LSQR via subroutine lsqrtest in lsqrTestModule.
!
! Maintained by Michael Saunders <saunders@stanford.edu>.
!
! 24 Oct 2007: Use real(8) instead of double precision or -r8.
!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

program lsqrTestProgram

  use   lsqrDataModule, only : dp
  use   lsqrTestModule, only : lsqrtest
  implicit none

  !---------------------------------------------------------------------
  ! This program calls lsqrtest(...) to generate a series of test problems
  ! Ax = b or Ax ~= b and solve them with LSQR.
  ! The matrix A is m x n.  It is defined by routines in lsqrTestModule.
  !
  ! 23 Sep 2007: First version of lsqrTestProgram.f90.
  ! 24 Oct 2007: Use real(dp) instead of compiler option -r8.
  !---------------------------------------------------------------------

  ! Local variables
  integer  :: m,n,nbar,ndamp,nduplc,npower,nout
  real(dp) :: damp


  nout   = 6
  open(nout,file='LSQR.txt',status='unknown')

  nbar   = 1000
  nduplc = 40

  m = 2*nbar        ! Over-determined systems
  n = nbar
  do ndamp = 2,7
     npower = ndamp
     damp   = 10.0**(-ndamp)
     call lsqrtest(m,n,nduplc,npower,damp,nout)
  end do

  m = nbar          ! Square systems
  n = nbar
  do ndamp = 2,7
     npower = ndamp
     damp   = 10.0**(-ndamp-6)
     call lsqrtest(m,n,nduplc,npower,damp,nout)
  end do

  m = nbar          ! Under-determined systems
  n = 2*nbar
  do ndamp = 2,7
     npower = ndamp
     damp   = 10.0**(-ndamp-6)
     call lsqrtest(m,n,nduplc,npower,damp,nout)
  end do

  close(nout)

end program lsqrTestProgram
