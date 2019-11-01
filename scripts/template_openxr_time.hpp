//## Copyright (c) 2017-2019 The Khronos Group Inc.
//## Copyright (c) 2019 Collabora, Ltd.
//##
//## Licensed under the Apache License, Version 2.0 (the "License");
//## you may not use this file except in compliance with the License.
//## You may obtain a copy of the License at
//##
//##     http://www.apache.org/licenses/LICENSE-2.0
//##
//## Unless required by applicable law or agreed to in writing, software
//## distributed under the License is distributed on an "AS IS" BASIS,
//## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//## See the License for the specific language governing permissions and
//## limitations under the License.
//## 
//## ---- Exceptions to the Apache 2.0 License: ----
//## 
//## As an exception, if you use this Software to generate code and portions of
//## this Software are embedded into the generated code as a result, you may
//## redistribute such product without providing attribution as would otherwise
//## be required by Sections 4(a), 4(b) and 4(d) of the License.
//## 
//## In addition, if you combine or link code generated by this Software with
//## software that is licensed under the GPLv2 or the LGPL v2.0 or 2.1
//## ("`Combined Software`") and if a court of competent jurisdiction determines
//## that the patent provision (Section 3), the indemnity provision (Section 9)
//## or other Section of the License conflicts with the conditions of the
//## applicable GPL or LGPL license, you may retroactively and prospectively
//## choose to deem waived or otherwise exclude such Section(s) of the License,
//## but only in their entirety and only with respect to the Combined Software.


//# set type = "Time"
//# set comparison_operators = ('<', '>', '<=', '>=', '==', '!=')
//# set invalid = ""
//# extends "template_openxr_wrapperclass.hpp"

//# block conversion_explicit_bool
//! True if this time is valid (positive)
OPENXR_HPP_CONSTEXPR explicit operator bool() const noexcept { return val_ > 0; }
//# endblock

//# block operator_negation
//! Unary negation: True if this /*{ type }*/ is invalid
OPENXR_HPP_CONSTEXPR bool operator!() const noexcept { return val_ <= 0; }
//# endblock

//# block extra_methods
//! Add a Duration to the current Time
Time& operator+=(Duration d) noexcept {
    val_ += d.get();
    return *this;
}

//! Subtract a Duration from the current Time
Time& operator-=(Duration d) noexcept {
    val_ -= d.get();
    return *this;
}
//# endblock

//# block extra_free_functions
//! The difference between two Time values is a Duration.
OPENXR_HPP_CONSTEXPR inline Duration operator-(Time lhs, Time rhs) noexcept { return Duration{lhs.get() - rhs.get()}; }

//! Subtract a Duration from a Time to get another Time
OPENXR_HPP_CONSTEXPR inline Time operator-(Time lhs, Duration rhs) noexcept { return Time{lhs.get() - rhs.get()}; }

//! Add a Duration to a Time to get another Time
OPENXR_HPP_CONSTEXPR inline Time operator+(Time lhs, Duration rhs) noexcept { return Time{lhs.get() + rhs.get()}; }

//# endblock
