#include <Poco/Util/Application.h>
#include <Poco/Path.h>
#include <Poco/FileStream.h>
#include <Poco/StreamCopier.h>
#include <Poco/File.h>
#include <Poco/URI.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/MD5Engine.h>
#include <Poco/JSON/Object.h>
#include <Poco/RegularExpression.h>
#include <Poco/Net/HTTPCookie.h>
#include <Poco/String.h>
#include <Poco/NumberParser.h>
#include <Poco/AutoPtr.h>

#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <thread>
#include <chrono>

#include "help/uploadPartHandler.hpp"
#include "help/mustache.hpp"
#include "help/cvcropper.hpp"
#include "help/cvblur.hpp"
#include "help/cvrotate.hpp"

#include "cropper.hpp"


namespace webcpp {
	namespace helloworld {

		void cropper::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
		{
			Poco::Util::Application &app = Poco::Util::Application::instance();
			Poco::URI uri = Poco::URI(request.getURI());
			if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST) {
				response.setContentType("application/json;charset=utf-8");
				Poco::JSON::Object data;
				Poco::RegularExpression regex("^/helloworld/cropper/upload/?$");


				if (regex.match(uri.getPath())) {
					webcpp::uploadPartHandler handler("uploadFile", "image/png|image/jpeg", app.config().getString("http.root"), app.config().getDouble("http.uploadMaxSize"));
					Poco::Net::HTMLForm form(request, request.stream(), handler);
					auto result = handler.getData();
					if (result[0].ok) {
						data.set("ok", 1);
						data.set("path", "/staticfile/index/" + result[0].webpath);
					} else {
						data.set("ok", 0);
						data.set("error", result[0].message);
					}

				} else {
					data.set("ok", 0);
					data.set("error", "fail request.");
				}

				data.stringify(response.send());
				return;
			}

			Poco::RegularExpression regex("^/helloworld/cropper/(cut|blur|gaussionblur|medianblur|bilateralfilter|rotate)/?$");
			std::vector<std::string> regexResult;
			if (regex.split(uri.getPath(), 0, regexResult) == 2 && request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET) {

				Poco::Net::HTMLForm form(request);
				std::string path = app.config().getString("http.root") + Poco::replace(form.get("src"), std::string("/staticfile/index"), std::string());

				if (Poco::File(path).exists()) {
					if (regexResult[1] == "cut") {
						int x = Poco::NumberParser::parse(form.get("x"))
							, y = Poco::NumberParser::parse(form.get("y"))
							, w = Poco::NumberParser::parse(form.get("w"))
							, h = Poco::NumberParser::parse(form.get("h"));
						webcpp::cvcropper cropper(path);
						cropper.create(x, y, w, h);
						response.setContentType("image/png");
						response.send() << cropper;

					} else if (regexResult[1] == "blur") {
						int kernel = Poco::NumberParser::parse(form.get("kernel", "7"));
						webcpp::cvblur blur(path);
						if (blur.blur(kernel)) {
							response.setContentType("image/png");
							response.send() << blur;
						} else {
							response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
							response.send() << "source image blur failed.";
						}

					} else if (regexResult[1] == "gaussionblur") {
						int kernel = Poco::NumberParser::parse(form.get("kernel", "7"));
						webcpp::cvblur blur(path);
						if (blur.gaussion(kernel)) {
							response.setContentType("image/png");
							response.send() << blur;
						} else {
							response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
							response.send() << "source image  GaussionBlur failed.";
						}

					} else if (regexResult[1] == "medianblur") {
						int kernel = Poco::NumberParser::parse(form.get("kernel", "7"));
						webcpp::cvblur blur(path);
						if (blur.median(kernel)) {
							response.setContentType("image/png");
							response.send() << blur;
						} else {
							response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
							response.send() << "source image medianBlur failed.";
						}

					} else if (regexResult[1] == "bilateralfilter") {
						int kernel = Poco::NumberParser::parse(form.get("kernel", "7"));
						webcpp::cvblur blur(path);
						if (blur.bilateralFilter(kernel)) {
							response.setContentType("image/png");
							response.send() << blur;
						} else {
							response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
							response.send() << "source image bilateralFilter failed.";
						}
					} else if (regexResult[1] == "rotate") {
						int degrees = Poco::NumberParser::parse(form.get("degrees", "30"));
						webcpp::cvrotate rotate(path, degrees);
						if (rotate.isReady()) {
							response.setContentType("image/png");
							response.send() << rotate;
						} else {
							response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
							response.send() << "source image bilateralFilter failed.";
						}
					}
				} else {
					response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
					response.send() << "source image is not exists.";
				}
				return;
			}



			Poco::Path tplPath(app.config().getString("http.tplDirectory", "/var/www/tpl"));
			tplPath.append("/example/cropper.html");

			if (!Poco::File(tplPath).exists()) {
				response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
				response.send() << tplPath.getFileName() + " is not exists.";
				return;
			}

			Poco::FileInputStream tplInput(tplPath.toString());
			std::string tplValue;
			Poco::StreamCopier::copyToString(tplInput, tplValue);

			Kainjow::Mustache::Data data = Kainjow::Mustache::Data::Type::Object;
			data.set("title", "图片裁剪、滤镜和旋转");
			response.setChunkedTransferEncoding(true);
			response.setContentType("text/html;charset=utf-8");

			Kainjow::Mustache tplEngine(tplValue);
			tplEngine.render(data, response.send());
		}

	}
}