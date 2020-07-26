/*
Copyright (C) 2000 The Exult Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef GUMP_BUTTON_H
#define GUMP_BUTTON_H

#include "Gump_widget.h"

#include <tuple>
#include <type_traits>

/*
 *  A pushable button on a gump:
 */
class Gump_button : public Gump_widget {
	UNREPLICATABLE_CLASS(Gump_button)

private:
	int pushed_button;      // 1 if in pushed state.

public:
	friend class Gump;
	Gump_button(Gump *par, int shnum, int px, int py,
	            ShapeFile shfile = SF_GUMPS_VGA)
		: Gump_widget(par, shnum, px, py, shfile), pushed_button(0)
	{  }
	// Is a given point on the checkmark?
	bool on_button(int mx, int my) const override {
		return on_widget(mx, my);
	}
	// What to do when 'clicked':
	virtual bool activate(int button) = 0;
	// Or double-clicked.
	virtual void double_clicked(int x, int y);
	virtual bool push(int button);  // Redisplay as pushed.
	virtual void unpush(int button);
	void paint() override;
	int get_pushed() {
		return pushed_button;
	}
	bool is_pushed() {
		return pushed_button != 0;
	}
	void set_pushed(int button) {
		pushed_button = button;
	}
	void set_pushed(bool set) {
		pushed_button = set ? 1 : 0;
	}
	virtual bool is_checkmark() const {
		return false;
	}

};

template <class Callable, class Tuple, size_t... Is>
inline auto call(Callable&& func, Tuple&& args,
                       std::index_sequence<Is...>) {
	return func(std::get<Is>(args)...);
}

template <typename Parent, typename Base, typename... Args>
class CallbackButtonBase : public Base {
public:
	using CallbackType = void (Parent::*)(Args...);
	using CallbackParams = std::tuple<Args...>;

	template <typename... Ts>
	CallbackButtonBase(Parent* par, CallbackType&& callback, CallbackParams&& params, Ts&&... args)
		: Base(par, std::forward<Ts>(args)...),
		  parent(par), on_click(std::forward<CallbackType>(callback)),
		  parameters(std::forward<CallbackParams>(params)) {}

	bool activate(int button) override {
		if (button != 1) return false;
		call([this](Args... args) {
				(parent->*on_click)(args...);
			}, parameters, std::make_index_sequence<sizeof...(Args)>{});
		return true;
	}

private:
	Parent* parent;
	CallbackType on_click;
	CallbackParams parameters;
};

template <typename Parent, typename Base>
class CallbackButtonBase<Parent, Base> : public Base {
public:
	using CallbackType = void (Parent::*)();
	using CallbackParams = std::tuple<>;

	template <typename... Ts>
	CallbackButtonBase(Parent* par, CallbackType&& callback, Ts&&... args)
		: Base(par, std::forward<Ts>(args)...),
		  parent(par), on_click(std::forward<CallbackType>(callback)) {}

	bool activate(int button) override {
		if (button != 1) return false;
		(parent->*on_click)();
		return true;
	}

private:
	Parent* parent;
	CallbackType on_click;
};

template <typename Parent, typename... Args>
using CallbackButton = CallbackButtonBase<Parent, Gump_button, Args...>;

#endif
