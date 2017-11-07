!
! Copyright (c) 2017, Lawrence Livermore National Security, LLC.
! 
! Produced at the Lawrence Livermore National Laboratory
! 
! LLNL-CODE-741217
! 
! All rights reserved.
!
! This file is part of Axom.
! 
! For details about use and distribution, please read axom/LICENSE.
!

program spio_preserve_write_read
  use axom_sidre
  use axom_spio
  use fruit
  use iso_c_binding
  implicit none

  include 'mpif.h'

  integer i
  integer mpierr
  integer num_files
  integer testvalue1, testvalue2, testvaluex
  integer num_elems1, num_elems2
  integer return_val
  integer my_rank, num_ranks, num_output
  integer ival
  integer, pointer :: i1_vals(:), i2_vals(:)
  character(80) :: s0, s1
  real(C_DOUBLE) :: dval

  type(SidreDataStore) ds1, ds2
  type(SidreGroup) root1, root2
  type(SidreGroup) flds, flds2, fldsx
  type(SidreGroup) ga, gb
  type(SidreView)  view1, view2, viewx

  type(SidreDataStore) dsextra
  type(SidreGroup) extra_root, extra, child
  type(SidreView) view

  type(iomanager) writer, reader

  call mpi_init(mpierr)

  call mpi_comm_rank(MPI_COMM_WORLD, my_rank, mpierr)
  call mpi_comm_size(MPI_COMM_WORLD, num_ranks, mpierr)

  num_output = num_ranks / 2 
  if (num_output == 0) then
     num_output = 1
  endif

! create a datastore and give it a small hierarchy of groups and views.
!
! the views are filled with repeatable nonsense data that will vary based
! on rank.
  ds1 = datastore_new()
  root1 = ds1%get_root()

  flds = root1%create_group("fields")
  flds2 = root1%create_group("fields2")

  ga = flds%create_group("a")
  gb = flds2%create_group("b")
  
  view1 = ga%create_view_scalar_int("i0", 101*my_rank)
  view2 = gb%create_view_and_allocate("i1", SIDRE_INT_ID, 10)

  call view2%get_data(i1_vals)

  do i = 0, 9
     i1_vals(i+1) = (i+10) * (404-my_rank-i)
  enddo

  ! contents of the datastore written to files with io_manager.
  num_files = num_output
  writer = iomanager_new(MPI_COMM_WORLD)

  call writer%write(root1, num_files, "F_out_spio_preserve_write_read", "sidre_hdf5")

  ! Extra stuff to exercise preserve_contents option
  call mpi_barrier(MPI_COMM_WORLD, mpierr)
  dsextra = datastore_new()
  extra_root = dsextra%get_root()
  extra = extra_root%create_group("extra")
  view = extra%create_View_Scalar("dval", 1.1d0)
  child = extra%create_group("child")
  view = child%create_view_scalar("ival", 7)
  view = child%create_view_string("word0", "hello")
  view = child%create_view_string("word1", "world")

  call writer%write(extra, num_files, "F_out_spio_extra", "sidre_hdf5")

  call mpi_barrier(MPI_COMM_WORLD, mpierr)

  ! create another datastore that holds nothing but the root group.
  ds2 = datastore_new()

  ! read from the files that were written above.
  reader = iomanager_new(MPI_COMM_WORLD)

  root2 = ds2%get_root()
  call reader%read(root2, "F_out_spio_preserve_write_read.root")

  ! verify that the contents of ds2 match those written from ds.
  return_val = 0
  if (.not. root2%is_equivalent_to(root1)) then
     return_val = 1 
  endif

  view1 = root1%get_view("fields/a/i0")
  view2 = root2%get_view("fields/a/i0")
  testvalue1 = view1%get_data_int()
  testvalue2 = view1%get_data_int()

  if (testvalue1 .ne. testvalue2) then
     return_val = 1
  endif

  view1 = root1%get_view("fields2/b/i1")
  view2 = root2%get_view("fields2/b/i1")

  num_elems1 = view1%get_num_elements()
  num_elems2 = view2%get_num_elements()
  if (num_elems1 .ne. num_elems2) then
     return_val = 1
  else
     call view1%get_data(i1_vals)
     call view2%get_data(i2_vals)
     if (any(i1_vals.ne.i2_vals)) then
        return_val = 1
     endif
  endif

  ! Read in extra file while preserving contents
  fldsx = root2%get_group("fields");
  call reader%read(fldsx, "F_out_spio_extra.root", .true.)

  ! Test one of the pre-existing Views to show it's unchanged.
  viewx = fldsx%get_view("a/i0")
  testvaluex = viewx%get_data_int()
  if (testvaluex .ne. testvalue2) then
     return_val = 1
  endif

  ! Test the data from the extra file.
  viewx = fldsx%get_view("dval")
  dval = viewx%get_data_double()

  if (dval .lt. 1.0000009 .or. dval .gt. 1.1000001) then
     return_val = 1
  endif

  viewx = fldsx%get_view("child/ival")
  ival = viewx%get_data_int() 

  if (ival .ne. 7) then
     return_val = 1
  endif

  viewx = fldsx%get_view("child/word0")
  call viewx%get_string(s0)
  call assert_equals(s0, "hello")

  viewx = fldsx%get_view("child/word1")
  call viewx%get_string(s1)
  call assert_equals(s1, "world")

  

  call ds1%delete()
  call ds2%delete()
  call dsextra%delete()

  call mpi_finalize(mpierr)

  call exit(return_val)
end program spio_preserve_write_read

