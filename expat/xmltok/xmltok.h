#ifndef XmlTok_INCLUDED
#define XmlTok_INCLUDED 1

#ifdef __cplusplus
extern "C" {
#endif

#ifndef XMLTOKAPI
#define XMLTOKAPI /* as nothing */
#endif

/* The following tokens may be returned by both XmlPrologTok and XmlContentTok */
#define XML_TOK_NONE -4    /* The string to be scanned is empty */
#define XML_TOK_TRAILING_CR -3 /* A CR at the end of the scan;
                                  might be part of CRLF sequence */ 
#define XML_TOK_PARTIAL_CHAR -2 /* only part of a multibyte sequence */
#define XML_TOK_PARTIAL -1 /* only part of a token */
#define XML_TOK_INVALID 0

/* The following tokens are returned by XmlContentTok; some are also
  returned by XmlAttributeValueTok and XmlEntityTok */

#define XML_TOK_START_TAG_WITH_ATTS 1
#define XML_TOK_START_TAG_NO_ATTS 2
#define XML_TOK_EMPTY_ELEMENT_WITH_ATTS 3 /* empty element tag <e/> */
#define XML_TOK_EMPTY_ELEMENT_NO_ATTS 4
#define XML_TOK_END_TAG 5
#define XML_TOK_DATA_CHARS 6
#define XML_TOK_DATA_NEWLINE 7
#define XML_TOK_CDATA_SECTION 8
#define XML_TOK_ENTITY_REF 9
#define XML_TOK_CHAR_REF 10     /* numeric character reference */

/* The following tokens may be returned by both XmlPrologTok and XmlContentTok */
#define XML_TOK_PI 11      /* processing instruction */
#define XML_TOK_COMMENT 12
#define XML_TOK_BOM 13     /* Byte order mark */

/* The following tokens are returned only by XmlPrologTok */
#define XML_TOK_INSTANCE_START 14
#define XML_TOK_PROLOG_S 15
#define XML_TOK_DECL_OPEN 16 /* <!foo */
#define XML_TOK_DECL_CLOSE 17 /* > */
#define XML_TOK_NAME 18
#define XML_TOK_NMTOKEN 19
#define XML_TOK_POUND_NAME 20 /* #name */
#define XML_TOK_OR 21 /* | */
#define XML_TOK_PERCENT 22
#define XML_TOK_OPEN_PAREN 23
#define XML_TOK_CLOSE_PAREN 24
#define XML_TOK_OPEN_BRACKET 25
#define XML_TOK_CLOSE_BRACKET 26
#define XML_TOK_LITERAL 27
#define XML_TOK_PARAM_ENTITY_REF 28

/* The following occur only in element type declarations */
#define XML_TOK_COMMA 29
#define XML_TOK_NAME_QUESTION 30 /* name? */
#define XML_TOK_NAME_ASTERISK 31 /* name* */
#define XML_TOK_NAME_PLUS 32 /* name+ */
#define XML_TOK_COND_SECT_OPEN 33 /* <![ */
#define XML_TOK_COND_SECT_CLOSE 34 /* ]]> */
#define XML_TOK_CLOSE_PAREN_QUESTION 35 /* )? */
#define XML_TOK_CLOSE_PAREN_ASTERISK 36 /* )* */
#define XML_TOK_CLOSE_PAREN_PLUS 37 /* )+ */


#define XML_N_STATES 2
#define XML_PROLOG_STATE 0
#define XML_CONTENT_STATE 1

#define XML_N_LITERAL_TYPES 2
#define XML_ATTRIBUTE_VALUE_LITERAL 0
#define XML_ENTITY_VALUE_LITERAL 1

#define XML_N_INTERNAL_ENCODINGS 1
#define XML_UTF8_ENCODING 0
#if 0
#define XML_UTF16_ENCODING 1
#define XML_UCS4_ENCODING 2
#endif

#define XML_MAX_BYTES_PER_CHAR 4

typedef struct position {
  /* first line and first column are 0 not 1 */
  unsigned long lineNumber;
  unsigned long columnNumber;
} POSITION;

typedef struct {
  const char *name;
  const char *valuePtr;
  const char *valueEnd;
  char containsRef;
} ATTRIBUTE;

struct encoding;
typedef struct encoding ENCODING;

struct encoding {
  int (*scanners[XML_N_STATES])(const ENCODING *,
			        const char *,
			        const char *,
			        const char **);
  int (*literalScanners[XML_N_LITERAL_TYPES])(const ENCODING *,
					      const char *,
					      const char *,
					      const char **);
  int (*sameName)(const ENCODING *,
	          const char *, const char *);
  int (*nameMatchesAscii)(const ENCODING *,
			  const char *, const char *);
  int (*nameLength)(const ENCODING *, const char *);
  int (*getAtts)(const ENCODING *enc, const char *ptr,
	         int attsMax, ATTRIBUTE *atts);
  int (*charRefNumber)(const ENCODING *enc, const char *ptr);
  void (*updatePosition)(const ENCODING *,
			 const char *ptr,
			 const char *end,
			 POSITION *);
  int (*isPublicId)(const ENCODING *enc, const char *ptr, const char *end,
		    const char **badPtr);
  int (*encode)(const ENCODING *enc,
		int charNum,
		char *buf);
  void (*convert[XML_N_INTERNAL_ENCODINGS])(const ENCODING *enc,
					    const char **fromP,
					    const char *fromLim,
					    char **toP,
					    const char *toLim);
  int minBytesPerChar;
};

/*
Scan the string starting at ptr until the end of the next complete token,
but do not scan past eptr.  Return an integer giving the type of token.

Return XML_TOK_NONE when ptr == eptr; nextTokPtr will not be set.

Return XML_TOK_PARTIAL when the string does not contain a complete token;
nextTokPtr will not be set.

Return XML_TOK_INVALID when the string does not start a valid token; nextTokPtr
will be set to point to the character which made the token invalid.

Otherwise the string starts with a valid token; nextTokPtr will be set to point
to the character following the end of that token.

Each data character counts as a single token, but adjacent data characters
may be returned together.  Similarly for characters in the prolog outside
literals, comments and processing instructions.
*/


#define XmlTok(enc, state, ptr, end, nextTokPtr) \
  (((enc)->scanners[state])(enc, ptr, end, nextTokPtr))

#define XmlPrologTok(enc, ptr, end, nextTokPtr) \
   XmlTok(enc, XML_PROLOG_STATE, ptr, end, nextTokPtr)

#define XmlContentTok(enc, ptr, end, nextTokPtr) \
   XmlTok(enc, XML_CONTENT_STATE, ptr, end, nextTokPtr)

/* This is used for performing a 2nd-level tokenization on
the content of a literal that has already been returned by XmlTok. */ 

#define XmlLiteralTok(enc, literalType, ptr, end, nextTokPtr) \
  (((enc)->literalScanners[literalType])(enc, ptr, end, nextTokPtr))

#define XmlAttributeValueTok(enc, ptr, end, nextTokPtr) \
   XmlLiteralTok(enc, XML_ATTRIBUTE_VALUE_LITERAL, ptr, end, nextTokPtr)

#define XmlEntityValueTok(enc, ptr, end, nextTokPtr) \
   XmlLiteralTok(enc, XML_ENTITY_VALUE_LITERAL, ptr, end, nextTokPtr)

#define XmlSameName(enc, ptr1, ptr2) (((enc)->sameName)(enc, ptr1, ptr2))

#define XmlNameMatchesAscii(enc, ptr1, ptr2) \
  (((enc)->nameMatchesAscii)(enc, ptr1, ptr2))

#define XmlNameLength(enc, ptr) \
  (((enc)->nameLength)(enc, ptr))

#define XmlGetAttributes(enc, ptr, attsMax, atts) \
  (((enc)->getAtts)(enc, ptr, attsMax, atts))

#define XmlCharRefNumber(enc, ptr) \
  (((enc)->charRefNumber)(enc, ptr))

#define XmlUpdatePosition(enc, ptr, end, pos) \
  (((enc)->updatePosition)(enc, ptr, end, pos))

#define XmlIsPublicId(enc, ptr, end, badPtr) \
  (((enc)->isPublicId)(enc, ptr, end, badPtr))

#define XmlEncode(enc, ch, buf) \
  (((enc)->encode)(enc, ch, buf))

#define XmlConvert(enc, targetEnc, fromP, fromLim, toP, toLim) \
  (((enc)->convert[targetEnc])(enc, fromP, fromLim, toP, toLim))

typedef struct {
  ENCODING initEnc;
  const ENCODING **encPtr;
} INIT_ENCODING;

int XMLTOKAPI XmlParseXmlDecl(int isGeneralTextEntity,
			      const ENCODING *enc,
			      const char *ptr,
	  		      const char *end,
			      const char **badPtr,
			      const char **versionPtr,
			      const char **encodingNamePtr,
			      const ENCODING **namedEncodingPtr,
			      int *standalonePtr);

void XMLTOKAPI XmlInitEncoding(INIT_ENCODING *, const ENCODING **);
const ENCODING XMLTOKAPI *XmlGetInternalEncoding(int);

#ifdef __cplusplus
}
#endif

#endif /* not XmlTok_INCLUDED */
