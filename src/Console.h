// alegrinho version
#ifndef CONSOLE_H
#define CONSOLE_H

#include <vector>
#include <string>
#include "Alegrinho.h"


namespace CONSOLE 
{

	const float DEFAULT_LINE_HEIGHT = 18.0f;
	const int SCROLL_DELAY = 3;
	const float FADE_SPEED = 0.05f;
	const float MIN_OPACITY = 0.25f;
	const float RESOLUTION = 2.0f;
	const float BUTTON_WIDTH = 22.0f;
	
}


class ChatString
{
	
	private:
	
	public:
		std::string text;
		std::string user_name;
		ColorRGBA user_color;
		ColorRGBA text_color;

		ChatString();
		ChatString(const char * text, const char * user_name = "", const ColorRGBA & user_color = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f), const ColorRGBA & text_color = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f) );
		ChatString(const std::string & text, const std::string & user_name = "", const ColorRGBA & user_color = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f), const ColorRGBA & text_color = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f) );
		
		//int get_text_width(ALLEGRO_FONT * font);
		
};


class Console
{

	private:
		std::vector<ChatString> Message;
		std::vector<ChatString> pendingMessage;
		
		float scaled_width, scaled_height;
		Framebuffer text_texture;
		bool need_texture_update;
		
		//ALLEGRO_MUTEX * mutex;
		
		
		void add_message_internal(const ChatString & message);
	
	public:
		bool active;
		float x, y;
		float width, height;
		float scale;
		int fontSize;
		int fade_time, time_on_screen;
		int scroll_position;
		char scroll_delay;
		float opacity;
		bool always_opaque;
		bool visible;
		bool full_hide;
		
		std::string inputText, visibleInputText;
		bool inputFocus;
		bool need_visible_input_text_update;
		bool show_input_vertical_line;
		int8_t input_vertical_line_time;
		
		// customization
		Bitmap * sprite;
		float _9slice_scale;
		float _9slice_width;
		Font * font;
		bool shadow_text;
		bool borders_outside;
		//std::string title;
		
		
		Console();
		~Console();
		void set(float x, float y, float width, float height, bool always_opaque, float scale = 1.0f, int fontSize = 0);
		void destroy();
		void reset();
		void hide();
		
		void draw();
		
		void clear_messages();
		void add_message(const ChatString & message);
		int update(bool ignore_inputs = false);
		void scroll_up();
		void scroll_down(int lines = 1);
		void scroll_to_top();
		void scroll_to_bottom();
		
		int get_max_visible_messages();
		bool is_empty();
		bool mouse_inside();
		bool currently_visible();
		
		bool mouse_over_up_button();
		bool mouse_over_down_button();
		
		bool up_button_pressed();
		bool down_button_pressed();
		
		// input box
		void check_copy_paste();
		void read_text_input();
		void update_visible_input_text();
		
		float get_height();
		float get_input_box_height();
		void get_dimensions(float & x, float & y, float & width, float & height);
		void get_bounds(float & left, float & top, float & right, float & bottom);
		
		void debug_printf();

};
#endif
