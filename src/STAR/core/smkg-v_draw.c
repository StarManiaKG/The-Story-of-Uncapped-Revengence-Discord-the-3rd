
#error "This file currently shouldn't be compiled."

Draw::TextElement& Draw::TextElement::parse(std::string_view raw)
{
	static const srb2::HashMap<std::string_view, char> translation = {
#define BUTTON(str, lower_bits) \
		{str,             0xB0 | lower_bits},\
		{str "_animated", 0xA0 | lower_bits},\
		{str "_pressed",  0x90 | lower_bits}

		BUTTON("up", sb_up),
		BUTTON("down", sb_down),
		BUTTON("right", sb_right),
		BUTTON("left", sb_left),

		BUTTON("lua1", sb_lua1),
		BUTTON("lua2", sb_lua2),
		BUTTON("lua3", sb_lua3),

		BUTTON("r", sb_r),
		BUTTON("l", sb_l),
		BUTTON("start", sb_start),

		BUTTON("a", sb_a),
		BUTTON("b", sb_b),
		BUTTON("c", sb_c),

		BUTTON("x", sb_x),
		BUTTON("y", sb_y),
		BUTTON("z", sb_z),

#undef BUTTON

		{"large", 0xEB},

		{"box", 0xEC},
		{"box_pressed", 0xED},
		{"box_animated", 0xEE},

		{"white", 0x80},
		{"purple", 0x81},
		{"yellow", 0x82},
		{"green", 0x83},
		{"blue", 0x84},
		{"red", 0x85},
		{"gray", 0x86},
		{"orange", 0x87},
		{"sky", 0x88},
		{"lavender", 0x89},
		{"gold", 0x8A},
		{"aqua", 0x8B},
		{"magenta", 0x8C},
		{"pink", 0x8D},
		{"brown", 0x8E},
		{"tan", 0x8F},
	};

	// When we encounter a Saturn button, what gamecontrol does it represent?
	static const srb2::HashMap<char, gamecontrols_e> inputdefinition = {
		{sb_up, gc_up},
		{sb_down, gc_down},
		{sb_right, gc_right},
		{sb_left, gc_left},

		{sb_lua1, gc_lua1},
		{sb_lua2, gc_lua2},
		{sb_lua3, gc_lua3},

		{sb_r, gc_r},
		{sb_l, gc_l},
		{sb_start, gc_start},

		{sb_a, gc_a},
		{sb_b, gc_b},
		{sb_c, gc_c},

		{sb_x, gc_x},
		{sb_y, gc_y},
		{sb_z, gc_z},
	};

	// What physical binds should appear as Saturn icons anyway?
	// (We don't have generic binds for stick/dpad directions, so
	// using the existing arrow graphics is the best thing here.)
	static const srb2::HashMap<INT32, char> prettyinputs = {
		{KEY_UPARROW, sb_up},
		{KEY_DOWNARROW, sb_down},
		{KEY_LEFTARROW, sb_left},
		{KEY_RIGHTARROW, sb_right},
		{nc_hatup, sb_up},
		{nc_hatdown, sb_down},
		{nc_hatleft, sb_left},
		{nc_hatright, sb_right},
		{nc_lsup, sb_up},
		{nc_lsdown, sb_down},
		{nc_lsleft, sb_left},
		{nc_lsright, sb_right},
	};

	string_.clear();
	string_.reserve(raw.size());

	using std::size_t;
	using std::string_view;

	for (;;)
	{
		size_t p = raw.find('<');

		// Copy characters until the start tag
		string_.append(raw.substr(0, p));

		if (p == raw.npos)
		{
			break; // end of string
		}

		raw.remove_prefix(p);

		// Find end tag
		p = raw.find('>');

		if (p == raw.npos)
		{
			break; // no end tag
		}

		string_view code = raw.substr(1, p - 1);

		if (code == "dpad" || code == "dpad_pressed" || code == "dpad_animated")
		{
			// SPECIAL: Generic button that we invoke explicitly, not via gamecontrol reference.
			// If we ever add anything else to this category, I promise I will create a real abstraction,
			// but for now, just hardcode the character replacements and pray for forgiveness.

			string_.push_back(0xEF); // Control code: "switch to descriptive input mode"
			string_.push_back(0xEB); // Control code: "large button"
			if (code == "dpad")
				string_.push_back(0xBC);
			else if (code == "dpad_pressed")
				string_.push_back(0x9C);
			else
				string_.push_back(0xAC);
		}
		else if (auto it = translation.find(code); it != translation.end()) // This represents a gamecontrol, turn into Saturn button or generic button.
		{
			UINT8 localplayer = R_GetViewNumber();

			if (as_.has_value())
			{
				UINT8 indexedplayer = as_.value();
				for (UINT8 i = 0; i < MAXSPLITSCREENPLAYERS; i++)
				{
					if (P_IsLocalPlayer(&players[indexedplayer]))
					{
						localplayer = i;
						break;
					}
				}
			}

			// This isn't how v_video.cpp checks for buttons and I don't know why.
			if (cv_descriptiveinput[localplayer].value && ((it->second & 0xF0) != 0x80)) // Should we do game control translation?
			{
				if (auto id = inputdefinition.find(it->second & (~0xB0)); id != inputdefinition.end()) // This is a game control, do descriptive input translation!
				{
					// Grab our local controls  - if pid set in the call to parse(), use stplyr's controls
					INT32 bind = G_FindPlayerBindForGameControl(localplayer, id->second);

					// EXTRA: descriptiveinput values above 1 translate binds back to Saturn buttons,
					// with various modes for various fucked up 6bt pads
					srb2::HashMap<INT32, char> padconfig = {};
					switch (cv_descriptiveinput[localplayer].value)
					{
						case 1:
							padconfig = standardpad;
							break;
						case 2:
							padconfig = flippedpad;
							break;
						case 3:
						{
							// Most players will map gc_L to their physical L button,
							// and gc_R to their physical R button. Assuming this is
							// true, try to guess their physical layout based on what
							// they've chosen.

							INT32 leftbumper = G_FindPlayerBindForGameControl(localplayer, gc_l);
							INT32 rightbumper = G_FindPlayerBindForGameControl(localplayer, gc_r);

							if (leftbumper == nc_lb && rightbumper == nc_lt)
							{
								padconfig = saturntypeA;
							}
							else if (leftbumper == nc_lt && rightbumper == nc_rt)
							{
								padconfig = saturntypeB;
							}
							else if (leftbumper == nc_lb && rightbumper == nc_rb)
							{
								padconfig = saturntypeC;
							}
							else if (leftbumper == nc_ls && rightbumper == nc_lb)
							{
								padconfig = saturntypeE;
							}
							else if (leftbumper == nc_rs && rightbumper == nc_lt)
							{
								padconfig = saturntypeE; // Not a typo! Users might bind a Hori layout pad to either bumpers or triggers
							}
							else
							{
								padconfig = saturntypeA; // :( ???
							}
							break;
						}
						case 4:
							padconfig = saturntypeA;
							break;
						case 5:
							padconfig = saturntypeB;
							break;
						case 6:
							padconfig = saturntypeC;
							break;
						case 7:
							padconfig = saturntypeD;
							break;
						case 8:
							padconfig = saturntypeE;
							break;
					}

					if (auto pretty = prettyinputs.find(bind); pretty != prettyinputs.end()) // Gamepad direction or keyboard arrow, use something nice-looking
					{
						string_.push_back((it->second & 0xF0) | pretty->second); // original invocation has the animation bits, but the glyph bits come from the table
					}
					else if (auto pad = padconfig.find(bind); pad != padconfig.end())
					{
						// If high bits are set, this is meant to be a generic button.
						if (pad->second & 0xF0)
						{
							string_.push_back(0xEF); // Control code: "switch to descriptive input mode" - buttons will draw as generics
							string_.push_back(0xEB); // Control code: "large button"
						}

						// Clear high bits so we can add animation bits back cleanly.
						pad->second = pad->second & (0x0F);

						// original invocation has the animation bits, but the glyph bits come from the table
						string_.push_back((it->second & 0xF0) | pad->second);
					}
					else
					{
						UINT8 fragment = (it->second & 0xB0);
						UINT8 code = '\xEE'; // Control code: "toggle boxed drawing"

						if (fragment == 0xA0)
							code = '\xED'; // ... but animated
						else if (fragment == 0x90)
							code = '\xEC'; // ... but pressed

						string_.push_back(code);

						if (bind == -1)
							string_.append("N/A");
						else
							string_.append((G_KeynumToShortString(bind)));

						string_.push_back(code);
					}
				}
				else // This is a color code or some other generic glyph, treat it as is.
				{
					string_.push_back(it->second); // replace with character code
				}
			}
			else // We don't care whether this is a generic glyph, because input translation isn't on.
			{
				string_.push_back(it->second); // replace with character code
			}
		}
		else
		{
			string_.append(raw.substr(0, p + 1)); // code not found, leave text verbatim
		}

		raw.remove_prefix(p + 1); // past end of tag
	}

	return *this;
}
