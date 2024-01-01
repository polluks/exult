/*
 * Copyright (C) 2016-2018 - Marzo Sette Torres Junior
 * Copyright (C) 2021-2022 - The Exult Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

// This reader removes a lot of warnings of C-style casts. It should be included
// after GTK+ and GDK headers.
// No, I am not proud of this, but it works.

#ifndef INCL_GTK_REDEFINES
#define INCL_GTK_REDEFINES  1

#include <cstring>
#include <type_traits>

// Do casts from pointers without too much undefined behavior.
template <typename To, typename From>
To safe_pointer_cast(From pointer) {
	// NOLINTNEXTLINE(bugprone-sizeof-expression)
	constexpr const size_t SizeFrom = sizeof(From);
	// NOLINTNEXTLINE(bugprone-sizeof-expression)
	constexpr const size_t SizeTo = sizeof(To);
	static_assert(std::is_pointer<From>::value && std::is_pointer<To>::value && SizeFrom == SizeTo, "Pointer sizes do not match");
	To output;
	std::memcpy(static_cast<void*>(&output),
				static_cast<void*>(&pointer),
				SizeFrom);
	return output;
}

namespace {
template <typename T1, typename T2>
struct gtk_cast_internal {
	static T1 *cast(T2 *obj) {
		return safe_pointer_cast<T1 *>(obj);
	}
};

template <typename T>
struct gtk_cast_internal<T, T> {
	static T *cast(T *obj) {
		return obj;
	}
};
}

template <typename T1, typename T2>
inline T1 *gtk_cast(T2 *obj) {
	return gtk_cast_internal<T1, T2>::cast(obj);
}

#if defined(_G_TYPE_CIC) || defined(_G_TYPE_CCC)
#  undef _G_TYPE_CIC
#  undef _G_TYPE_CCC
#  undef _G_TYPE_CIT
#  undef _G_TYPE_CIFT
#  ifndef G_DISABLE_CAST_CHECKS
#    define _G_TYPE_CIC(ip, gt, ct)     gtk_cast<ct>(g_type_check_instance_cast(gtk_cast<GTypeInstance>((ip)), gt))
#    define _G_TYPE_CCC(cp, gt, ct)     gtk_cast<ct>(g_type_check_class_cast(gtk_cast<GTypeClass>((cp)), gt))
#    define _G_TYPE_CIT(ip, gt)                     (g_type_check_instance_is_a(gtk_cast<GTypeInstance>((ip)), gt))
#    define _G_TYPE_CIFT(ip, ft)                    (g_type_check_instance_is_fundamentally_a(gtk_cast<GTypeInstance>((ip)), ft))
#  else /* G_DISABLE_CAST_CHECKS */
#    define _G_TYPE_CIC(ip, gt, ct)     gtk_cast<ct>((ip))
#    define _G_TYPE_CCC(cp, gt, ct)     gtk_cast<ct>((cp))
#  endif /* G_DISABLE_CAST_CHECKS */
#endif /* defined(_G_TYPE_CIC) || defined(_G_TYPE_CCC) */

#ifdef _G_TYPE_IGI
#  undef _G_TYPE_IGI
#  define _G_TYPE_IGI(ip, gt, ct)       gtk_cast<ct>(g_type_interface_peek(gtk_cast<GTypeInstance>((ip))->g_class, gt))
#endif /* _G_TYPE_IGI */

#ifdef G_CALLBACK
#  undef G_CALLBACK
#  define G_CALLBACK(f)       (safe_pointer_cast<GCallback>((f)))
#endif /* G_CALLBACK */

#ifdef G_TYPE_MAKE_FUNDAMENTAL
#  undef G_TYPE_MAKE_FUNDAMENTAL
#define G_TYPE_MAKE_FUNDAMENTAL(x)      (static_cast<GType>((x) << G_TYPE_FUNDAMENTAL_SHIFT))
#endif /* G_TYPE_MAKE_FUNDAMENTAL */

#if defined(g_list_previous) || defined(g_list_next)
#  undef g_list_previous
#  undef g_list_next
#define g_list_previous(list)           ((list) ? (gtk_cast<GList>((list))->prev) : nullptr)
#define g_list_next(list)               ((list) ? (gtk_cast<GList>((list))->next) : nullptr)
#endif /* defined(g_list_previous) || defined(g_list_next) */

#ifdef g_signal_connect
#  undef g_signal_connect
#define g_signal_connect(instance, detailed_signal, c_handler, data) \
	g_signal_connect_data ((instance), (detailed_signal), (c_handler), (data), nullptr, static_cast<GConnectFlags>(0))
#endif /* g_signal_connect */

#ifdef g_utf8_next_char
#  undef g_utf8_next_char
#define g_utf8_next_char(p)   ((p) + g_utf8_skip[*safe_pointer_cast<const guchar *>((p))])
#endif /* g_utf8_next_char */

#ifdef G_LOG_DOMAIN
#  undef G_LOG_DOMAIN
#  define G_LOG_DOMAIN    nullptr
#endif

#endif
