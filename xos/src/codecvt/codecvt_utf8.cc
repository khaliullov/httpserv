#include "codecvt_utf8.h"

namespace std {

template class codecvt_utf8<wchar_t, max_unicode_codepoint()>;
template class codecvt_utf8<wchar_t, max_unicode_codepoint(),
	little_endian>;
template class codecvt_utf8<wchar_t, max_unicode_codepoint(),
	generate_header>;
template class codecvt_utf8<wchar_t, max_unicode_codepoint(),
	codecvt_mode(generate_header | little_endian)>;
template class codecvt_utf8<wchar_t, max_unicode_codepoint(),
	consume_header>;
template class codecvt_utf8<wchar_t, max_unicode_codepoint(),
	codecvt_mode(consume_header | little_endian)>;
template class codecvt_utf8<wchar_t, max_unicode_codepoint(),
	codecvt_mode(consume_header | generate_header)>;
template class codecvt_utf8<wchar_t, max_unicode_codepoint(),
	codecvt_mode(consume_header | generate_header | little_endian)>;

template class codecvt_utf8<char16_t, max_unicode_codepoint()>;
template class codecvt_utf8<char16_t, max_unicode_codepoint(),
	little_endian>;
template class codecvt_utf8<char16_t, max_unicode_codepoint(),
	generate_header>;
template class codecvt_utf8<char16_t, max_unicode_codepoint(),
	codecvt_mode(generate_header | little_endian)>;
template class codecvt_utf8<char16_t, max_unicode_codepoint(),
	consume_header>;
template class codecvt_utf8<char16_t, max_unicode_codepoint(),
	codecvt_mode(consume_header | little_endian)>;
template class codecvt_utf8<char16_t, max_unicode_codepoint(),
	codecvt_mode(consume_header | generate_header)>;
template class codecvt_utf8<char16_t, max_unicode_codepoint(),
	codecvt_mode(consume_header | generate_header | little_endian)>;

template class codecvt_utf8<char32_t, max_unicode_codepoint()>;
template class codecvt_utf8<char32_t, max_unicode_codepoint(),
	little_endian>;
template class codecvt_utf8<char32_t, max_unicode_codepoint(),
	generate_header>;
template class codecvt_utf8<char32_t, max_unicode_codepoint(),
	codecvt_mode(generate_header | little_endian)>;
template class codecvt_utf8<char32_t, max_unicode_codepoint(),
	consume_header>;
template class codecvt_utf8<char32_t, max_unicode_codepoint(),
	codecvt_mode(consume_header | little_endian)>;
template class codecvt_utf8<char32_t, max_unicode_codepoint(),
	codecvt_mode(consume_header | generate_header)>;
template class codecvt_utf8<char32_t, max_unicode_codepoint(),
	codecvt_mode(consume_header | generate_header | little_endian)>;

} // namespace std
