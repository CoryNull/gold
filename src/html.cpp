#include "html.hpp"

#include "array.hpp"

#define DefineElementType(name)                 \
	name::name() : iHTML(#name, {}) {}            \
	name::name(const name& copy) : iHTML(copy) {} \
	name::name(varList args) : iHTML(#name, args) {}

#define DefineElementTypeTag(name, tag)         \
	name::name() : iHTML(tag, {}) {}              \
	name::name(const name& copy) : iHTML(copy) {} \
	name::name(varList args) : iHTML(tag, args) {}

namespace gold {
	namespace HTML {
		gold::object iHTML::proto = object({
			{"setAttributes", method(&iHTML::setAttributes)},
			{"getAttribute", method(&iHTML::getAttribute)},
			{"addElements", method(&iHTML::addElements)},
			{"removeElement", method(&iHTML::removeElement)},
		});

		iHTML::iHTML() : object(&proto) {
			setNull("tag");
			setArray("items", array());
			setObject("attr", object());
		}

		iHTML::iHTML(const iHTML& copy) : object(copy, &proto) {}

		iHTML::iHTML(const char* name, varList args)
			: object(&proto) {
			setString("tag", name);
			auto items = array();
			auto attr = object();
			for (auto it = args.begin(); it != args.end(); ++it) {
				if (it->isObject(proto))
					items.pushVar(*it);
				else if (it->isString())
					items.pushVar(*it);
				else if (it->isObject())
					attr = *it;
			}
			setArray("items", items);
			setObject("attr", attr);
		}

		gold::var iHTML::setAttributes(varList args) {
			auto attr = getObject("attr");
			auto object = args[0].getObject();
			if (object && attr) {
				attr->copy(*object);
				return gold::var(*attr);
			}
			return gold::var();
		}

		gold::var iHTML::getAttribute(varList) {
			auto attr = getObject("attr");
			if (attr) return gold::var(*attr);
			return gold::var();
		}

		gold::var iHTML::addElements(varList args) {
			auto items = getArray("items");
			for (auto it = args.begin(); it != args.end(); ++it) {
				if (it->isObject(proto))
					items->pushVar(*it);
				else if (it->isString())
					items->pushVar(*it);
			}
			return var();
		}

		gold::var iHTML::removeElement(varList args) {
			auto items = getArray("items");
			for (auto it = args.begin(); it != args.end(); ++it) {
				if (it->isObject(proto)) {
					auto in = items->find(*it);
					if (in != items->end()) items->erase(in);
				}
			}
			return var();
		}

		iHTML::operator string() {
			auto buffer = string();
			auto tag = getString("tag");
			auto attr = getObject("attr");
			auto items = getArray("items");
			buffer += "<" + tag;
			if (attr->getSize() > 0) buffer += " ";
			for (auto it = attr->begin(); it != attr->end(); ++it) {
				if (it->second.isBool()) {
					if (it->second.getBool()) buffer += it->first + " ";
				} else
					buffer +=
						it->first + "=\"" + it->second.getString() + "\" ";
			}
			buffer += ">";
			for (auto it = items->begin(); it != items->end(); ++it) {
				if (it->isObject(proto))
					buffer += (string)(*(iHTML*)it->getObject());
				else if (it->isString())
					buffer += (string)(*it);
			}
			buffer += "</" + tag + ">";
			return buffer;
		}

		iHTML& iHTML::operator+=(varList args) {
			auto items = getArray("items");
			auto attr = getObject("attr");
			for (auto it = args.begin(); it != args.end(); ++it) {
				if (it->isObject(proto))
					items->pushVar(*it);
				else if (it->isObject())
					attr->copy(*it->getObject());
			}
			return *this;
		}

		iHTML& iHTML::operator-=(varList args) {
			removeElement(args);
			return *this;
		}

		DefineElementTypeTag(hTemplate, "template")
		DefineElementTypeTag(hObject, "object")

		DefineElementType(html)
		DefineElementType(head)
		DefineElementType(body)

		DefineElementType(meta)
		DefineElementType(script)
		DefineElementType(style)

		DefineElementType(nav)
		DefineElementType(base)
		DefineElementType(br)
		DefineElementType(param)
		DefineElementType(link)
		DefineElementType(title)
		DefineElementType(span)
		DefineElementType(p)
		DefineElementType(a)
		DefineElementType(img)
		DefineElementType(area)
		DefineElementType(audio)
		DefineElementType(canvas)
		DefineElementType(embed)
		DefineElementType(source)
		DefineElementType(track)
		DefineElementType(video)
		DefineElementType(map)
		DefineElementType(input)
		DefineElementType(h1)
		DefineElementType(h2)
		DefineElementType(h3)
		DefineElementType(h4)
		DefineElementType(h5)
		DefineElementType(h6)

		DefineElementType(dl)
		DefineElementType(dt)
		DefineElementType(dd)
		DefineElementType(ol)
		DefineElementType(ul)
		DefineElementType(li)

		DefineElementType(adress)
		DefineElementType(article)
		DefineElementType(aside)
		DefineElementType(blockquote)
		DefineElementType(del)
		DefineElementType(div)
		DefineElementType(figure)
		DefineElementType(figcaption)
		DefineElementType(footer)
		DefineElementType(header)
		DefineElementType(hr)
		DefineElementType(ins)
		DefineElementType(main)
		DefineElementType(pre)
		DefineElementType(section)
		DefineElementType(bdi)
		DefineElementType(bdo)
		DefineElementType(cite)
		DefineElementType(data)

		DefineElementType(b)
		DefineElementType(abbr)
		DefineElementType(dfn)
		DefineElementType(q)
		DefineElementType(i)
		DefineElementType(u)
		DefineElementType(s)
		DefineElementType(small)
		DefineElementType(strong)
		DefineElementType(em)
		DefineElementType(mark)
		DefineElementType(rp)
		DefineElementType(sub)
		DefineElementType(rb)
		DefineElementType(rt)
		DefineElementType(ruby)
		DefineElementType(time)
		DefineElementType(wbr)

		DefineElementType(code)
		DefineElementType(kbd)
		DefineElementType(samp)
		DefineElementType(var)

		DefineElementType(form)
		DefineElementType(button)
		DefineElementType(datalist)
		DefineElementType(fieldlist)
		DefineElementType(label)
		DefineElementType(legend)
		DefineElementType(meter)
		DefineElementType(option)
		DefineElementType(optgroup)
		DefineElementType(output)
		DefineElementType(progress)
		DefineElementType(select)
		DefineElementType(textarea)

		DefineElementType(table)
		DefineElementType(tr)
		DefineElementType(th)
		DefineElementType(td)
		DefineElementType(colgroup)
		DefineElementType(col)
		DefineElementType(caption)
		DefineElementType(thead)
		DefineElementType(tbody)
		DefineElementType(tfoot)

		DefineElementType(iframe)

	}  // namespace HTML
}  // namespace gold