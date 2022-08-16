#include "imgui_internal.h"

using namespace ImGui;

void ImGui_RenderMultiline(ImVector<char> &d, const char *buffer, float maxWidth_)
{
	int size = 1;
	{
		auto *c = buffer;
		while (*c++ != 0)
			++size;
	}
		
	d.resize(size*2);
	
    ImGuiContext& g = *GImGui;
    
    ImFont* font = g.Font;

	auto *out = d.Data;
	auto *in = buffer;
	auto *space = out, *newline = out;
	
	char NEW_LINE = '\n';
	int bufferSize = 3; // what should this be?
	float maxWidth = maxWidth_ - bufferSize;

	float width = 0, widthSinceSpace = 0;
	
	while (*in != 0)
	{
		auto c = *in++;
		auto w =
			((int)c < font->IndexAdvanceX.Size ?
				font->IndexAdvanceX.Data[c] :
				font->FallbackAdvanceX);
			
		width += w;
		widthSinceSpace += w;
		
		if (c == ' ')
		{
			space = out;
			widthSinceSpace = 0;
		}
		else
		if (c == '\n')
		{
			space = newline = out;
			width = widthSinceSpace = 0;
		}

		if (width > maxWidth)
		{
			if (space != newline)
			{
				*space = NEW_LINE;
				width = widthSinceSpace + w;
				widthSinceSpace = w;
				newline = space;
				
				if (space == out)
				{
					out++;
					continue;
				}
			}
			else
			{
				*out++ = NEW_LINE;
				width = w;
				widthSinceSpace = 0;
				newline = out;
				space = out;
			}
		}
			
		*out++ = c;
	}
	
	*out++ = 0;
}

// Based on stb_to_utf8() from github.com/nothings/stb/
inline int ImGui_W2C(char* buf, int buf_size, unsigned int c)
{
    if (c < 0x80)
    {
        buf[0] = (char)c;
        return 1;
    }
    if (c < 0x800)
    {
        if (buf_size < 2) return 0;
        buf[0] = (char)(0xc0 + (c >> 6));
        buf[1] = (char)(0x80 + (c & 0x3f));
        return 2;
    }
    if (c < 0x10000)
    {
        if (buf_size < 3) return 0;
        buf[0] = (char)(0xe0 + (c >> 12));
        buf[1] = (char)(0x80 + ((c >> 6) & 0x3f));
        buf[2] = (char)(0x80 + ((c ) & 0x3f));
        return 3;
    }
    if (c <= 0x10FFFF)
    {
        if (buf_size < 4) return 0;
        buf[0] = (char)(0xf0 + (c >> 18));
        buf[1] = (char)(0x80 + ((c >> 12) & 0x3f));
        buf[2] = (char)(0x80 + ((c >> 6) & 0x3f));
        buf[3] = (char)(0x80 + ((c ) & 0x3f));
        return 4;
    }
    // Invalid code point, the max unicode is 0x10FFFF
    return 0;
}


void ImGui_RenderMultiline(ImVector<char> &d, ImVector<ImWchar> &r, int &rSize, ImVector<ImWchar> &w, float maxWidth_)
{
    ImGuiContext& g = *GImGui;
    ImFont* font = g.Font;

	r.resize(w.size()*2);
	d.resize(r.size()*4);
	
	rSize = 0;
	
	auto *outC = d.Data;
	auto *outW = r.Data;
	
	auto *in = w.Data;
	auto *spaceC = outC, *newlineC = outC;
	auto *spaceW = outW, *newlineW = outW;
	
	char NEW_LINE = '\n';
	int bufferSize = 3;
	float maxWidth = maxWidth_ - bufferSize;

	float width = 0, widthSinceSpace = 0;
	while (*in != 0)
	{
		auto c = *in++;
		auto w =
			((int)c < font->IndexAdvanceX.Size ?
				font->IndexAdvanceX.Data[c] :
				font->FallbackAdvanceX);
			
		width += w;
		widthSinceSpace += w;
		
		if (c == ' ')
		{
			spaceC = outC; spaceW = outW;
			widthSinceSpace = 0;
		}
		else
		if (c == '\n')
		{
			spaceC = newlineC = outC;
			spaceW = newlineW = outW;
			width = widthSinceSpace = 0;
		}

		if (width > maxWidth)
		{
			if (spaceC != newlineC)
			{
				*spaceC = NEW_LINE; *spaceW = NEW_LINE;
				
				width = widthSinceSpace + w;
				widthSinceSpace = w;
				
				newlineC = spaceC; newlineW = spaceW;
				
				if (spaceC == outC)
				{
					outC++;	outW++;
					rSize++;
					continue;
				}
			}
			else
			{
				*outC++ = NEW_LINE;	*outW++ = NEW_LINE;
				rSize++;
				
				width = w;
				widthSinceSpace = 0;
				
				newlineC = outC; newlineW = outW;
				spaceC = outC; spaceW = outW;
			}
		}
			
		*outW++ = c;
		rSize++;
		
		outC += ImGui_W2C(outC, 10, c);
	}
	
	*outC++ = 0; *outW++ = 0;
}

