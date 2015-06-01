!
! Copyright (c) 2015, Lawrence Livermore National Security, LLC.
! Produced at the Lawrence Livermore National Laboratory.
!
! All rights reserved.
!
! This source code cannot be distributed without permission and
! further review from Lawrence Livermore National Laboratory.
!
module dataview_mod
    use fstr_mod
    use datagroup_mod, only : datagroup
    use iso_c_binding
    
    type dataview
        type(C_PTR) obj
    contains
        procedure :: has_buffer => dataview_has_buffer
        procedure :: get_name => dataview_get_name
        procedure :: get_owning_group => dataview_get_owning_group
    end type dataview
    
    interface
        
        function atk_dataview_has_buffer(self) result(rv) bind(C, name="ATK_dataview_has_buffer")
            use iso_c_binding
            implicit none
            type(C_PTR), value :: self
            logical(C_BOOL) :: rv
        end function atk_dataview_has_buffer
        
        pure function atk_dataview_get_name(self) result(rv) bind(C, name="ATK_dataview_get_name")
            use iso_c_binding
            implicit none
            type(C_PTR), value :: self
            type(C_PTR) rv
        end function atk_dataview_get_name
        
        function atk_dataview_get_owning_group(self) result(rv) bind(C, name="ATK_dataview_get_owning_group")
            use iso_c_binding
            implicit none
            type(C_PTR), value :: self
            type(C_PTR) :: rv
        end function atk_dataview_get_owning_group
    end interface

contains
    
    function dataview_has_buffer(obj) result(rv)
        implicit none
        class(dataview) :: obj
        logical :: rv
        ! splicer begin
        rv = bool2logical(atk_dataview_has_buffer(obj%obj))
        ! splicer end
    end function dataview_has_buffer
    
    function dataview_get_name(obj) result(rv)
        implicit none
        class(dataview) :: obj
        character(kind=C_CHAR, len=1) :: rv
        type(C_PTR) :: rv_ptr
        ! splicer begin
        rv = fstr(atk_dataview_get_name(obj%obj))
        ! splicer end
    end function dataview_get_name
    
    function dataview_get_owning_group(obj) result(rv)
        implicit none
        class(dataview) :: obj
        type(datagroup) :: rv
        ! splicer begin
        rv%obj = atk_dataview_get_owning_group(obj%obj)
        ! splicer end
    end function dataview_get_owning_group

end module dataview_mod
