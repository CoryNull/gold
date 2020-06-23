#include "index.hpp"

#include <html.hpp>
#include <iostream>

#include "template.hpp"

namespace gg {
	using div = HTML::div;
	void setIndexRoute(database, server serv) {
		func getIndex = [](const list& args) -> gold::var {
			using namespace HTML;
			auto req = args[0].getObject<request>();
			auto res = args[1].getObject<response>();
			return res.end({getTemplate(
				req,
				{
					div({
						obj{{"class", "card pageCard text-light bg-dark"}},
						div(
							{obj{{"class", "card-body"}},
							 h5({
								 obj{{"class", "card-title"}},
								 "Welcome!",
							 }),
							 p({"This is one of the gold framework examples. "
									"This project will allow you to get started "
									"developing full-stack web applications with "
									"C++. A lot of the design philosophies used "
									"in this project are inspired by frameworks "
									"from NodeJS. Trying to make an organic, "
									"flexible, and powerful framework that will "
									"create great apps."}),
							 p({"This project makes REST requests over HTTP "
									"but can use WebSockets to deliver data. "
									"This project was meant to be a starting "
									"point but it's actually based off another "
									"project I'm working on. New features are "
									"likely going to be added to that project "
									"and sent down stream to this one."}),
							 p({"This project uses the Apache 2 License, "
									"which means you have to give credit to me. "
									"It's the least you could do. I also want to "
									"advertise that I do have a PayPal you can "
									"donate to, that will help me work on this "
									"project and spend more time on it."}),
							 a({atts{
										{"href", "https://www.paypal.me/corynil"}},
									"Donate Here!"})}),
					}),
				})});
		};

		serv.get({"/", getIndex});
	}
}  // namespace gg