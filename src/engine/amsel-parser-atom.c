#include "amsel-parser-atom.h"
#include "amsel-parser.h"
#include <libxml/parser.h>
#include "amsel-channel.h"

struct _AmselParserAtom
{
  GObject parent_instance;
};

static void amsel_parser_atom_iface_init (AmselParserInterface *iface);
G_DEFINE_TYPE_WITH_CODE (AmselParserAtom, amsel_parser_atom, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (AMSEL_TYPE_PARSER, amsel_parser_atom_iface_init))

/* enum { */
/*   PROP_0, */
/*   N_PROPS */
/* }; */

/* static GParamSpec *properties [N_PROPS]; */

AmselParserAtom *
amsel_parser_atom_new (void)
{
  return g_object_new (AMSEL_TYPE_PARSER_ATOM, NULL);
}

static void
amsel_parser_atom_finalize (GObject *object)
{
  /* AmselParserAtom *self = (AmselParserAtom *)object; */

  G_OBJECT_CLASS (amsel_parser_atom_parent_class)->finalize (object);
}

/* static void */
/* amsel_parser_atom_get_property (GObject    *object, */
/*                                 guint       prop_id, */
/*                                 GValue     *value, */
/*                                 GParamSpec *pspec) */
/* { */
/*   AmselParserAtom *self = AMSEL_PARSER_ATOM (object); */

/*   switch (prop_id) */
/*     { */
/*     default: */
/*       G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec); */
/*     } */
/* } */

/* static void */
/* amsel_parser_atom_set_property (GObject      *object, */
/*                                 guint         prop_id, */
/*                                 const GValue *value, */
/*                                 GParamSpec   *pspec) */
/* { */
/*   AmselParserAtom *self = AMSEL_PARSER_ATOM (object); */

/*   switch (prop_id) */
/*     { */
/*     default: */
/*       G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec); */
/*     } */
/* } */

static void
amsel_parser_atom_class_init (AmselParserAtomClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = amsel_parser_atom_finalize;
  /* object_class->get_property = amsel_parser_atom_get_property; */
  /* object_class->set_property = amsel_parser_atom_set_property; */
}

static void
amsel_parser_atom_init (AmselParserAtom *self)
{
}

#define HANDLE_CHANNEL_NODE(namestr, parsefunc) \
    (!xmlStrcmp (cur->name, BAD_CAST #namestr)) { \
      char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);\
      parsefunc (channel, str); \
      xmlFree (str);\
    }

#define HANDLE_ITEM_NODE(namestr, parsefunc) \
    (!xmlStrcmp (cur->name, BAD_CAST #namestr)) { \
      char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);\
      parsefunc (item, str); \
      xmlFree (str);\
    }
static void
amsel_parser_atom_parse_item (AmselParser *parser,
                              AmselEntry  *item,
                              xmlDocPtr    doc,
                              xmlNodePtr   cur)
{
  while (cur)
    {
      if HANDLE_ITEM_NODE (title, amsel_entry_set_title)
      // TODO: handle different types of content
      else if HANDLE_ITEM_NODE (content, amsel_entry_set_content)
      else if HANDLE_ITEM_NODE (id, amsel_entry_set_id)
      else if (!xmlStrcmp (cur->name, BAD_CAST "updated")) {
        char *str = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
        g_autoptr (GDateTime) updated = g_date_time_new_from_iso8601 (str, NULL);
        amsel_entry_set_updated_datetime (item, updated);
        xmlFree (str);
      }
      else if (!xmlStrcmp (cur->name, BAD_CAST "author")) {
        xmlNodePtr oldcur = cur;
        cur = cur->xmlChildrenNode;
        while (cur) {
          if HANDLE_ITEM_NODE (name, amsel_entry_set_author)
          cur = cur->next;
        }
        cur = oldcur;
      }
      cur = cur->next;
    }
}

static void
amsel_parser_atom_parse_channel (AmselParser  *parser,
                                 AmselChannel *channel,
                                 xmlDocPtr     doc,
                                 xmlNodePtr    cur)
{
  while (cur) {
    if HANDLE_CHANNEL_NODE (title, amsel_channel_set_title)
    else if (!xmlStrcmp (cur->name, BAD_CAST "link"))
      {
        xmlChar *href = xmlGetProp (cur, BAD_CAST "href");
        if (href) {
          xmlChar *rel = xmlGetProp (cur, BAD_CAST "rel");
          if (xmlStrEqual (rel, BAD_CAST "self"))
            amsel_channel_set_source (channel, (const char *) href);
          xmlFree (href);
          xmlFree (rel);
        }
      }
    else if (!xmlStrcmp (cur->name, BAD_CAST "entry"))
      {
        g_autoptr (AmselEntry) item = amsel_entry_new ();
        amsel_parser_atom_parse_item (parser, item, doc, cur->xmlChildrenNode);
        amsel_channel_add_entry (channel, item);
      }

    cur = cur->next;
  }
}

static void
amsel_parser_atom_internal_parse (AmselParser *parser,
                                  GPtrArray   *channels,
                                  xmlDocPtr    doc,
                                  xmlNodePtr   node)
{
  xmlNodePtr cur = node;

  while (cur)
    {
      AmselChannel *channel = amsel_channel_new ("");
      amsel_parser_atom_parse_channel (parser, channel, doc, cur->xmlChildrenNode);
      g_ptr_array_add (channels, channel);
      cur = cur->next;
    }
}

GPtrArray *
amsel_parser_atom_parse (AmselParser  *parser,
                         AmselRequest *request)
{
  g_return_val_if_fail (AMSEL_IS_PARSER (parser), NULL);

  xmlDocPtr doc;
  GPtrArray *channels;

  channels = g_ptr_array_new_with_free_func (g_object_unref);

  doc = xmlParseMemory (amsel_request_get_data (request),
                        amsel_request_get_size (request));

  xmlNodePtr root = xmlDocGetRootElement (doc);
  amsel_parser_atom_internal_parse (parser, channels, doc, root);
  xmlFreeDoc (doc);

  return channels;
}

static void
amsel_parser_atom_iface_init (AmselParserInterface *iface)
{
  iface->parse = amsel_parser_atom_parse;
}
