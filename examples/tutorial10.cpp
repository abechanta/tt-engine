#include <actor.h>
#include <asset.h>
#include <asset_handler.h>
#include <components/shape_component.h>
#include <boost/property_tree/ptree.hpp>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
using namespace tte;

namespace Tutorial10 {
	class DummyRenderer : public tte::Renderer2dInterface {
		virtual void drawPoint(Actor &a, const vector2 &point) const override {}
		virtual void drawPoints(Actor &a, const vector<vector2> &points) const override {}
		virtual void drawLine(Actor &a, const vector2 &lineS, const vector2 &lineE) const override {}
		virtual void drawLines(Actor &a, const vector<vector2> &lineS, const vector<vector2> &lineE) const override {}
		virtual void drawLines(Actor &a, const vector<vector2> &points, const vector<int32_t> &lines) const override {}

		virtual void drawRect(Actor &a, const vector2 &pos, const vector2 &size, const vector2 &anchor, const vec<bool, 2> &flip) const override {
			cout << __FUNCTION__ << endl;
			cout << "\t" << "pos: " << X(pos) << ", " << Y(pos) << endl;
			cout << "\t" << "size: " << X(size) << ", " << Y(size) << endl;
			cout << "\t" << "anchor: " << X(anchor) << ", " << Y(anchor) << endl;
			cout << "\t" << "flip: " << X(flip) << ", " << Y(flip) << endl;
		}
	};

	void test1() {
		cout << __FUNCTION__ << endl;
		auto chara = make_unique<Asset>(L"asset/tutorial10/chara.json", AssetHandler::typeJson);
		auto score = make_unique<Asset>(L"asset/tutorial10/score.json", AssetHandler::typeJson);
		cout << "--- load" << endl;
		chara->load();
		score->load();
		ptree props1, props2;
		props1.insert(props1.end(), chara->props().begin(), chara->props().end());
		props2.insert(props2.end(), score->props().begin(), score->props().end());
		auto sprite1 = make_unique<ShapeSprite>(props1.get_child("sprite"));
		auto text1 = make_unique<ShapeText>(props2.get_child("text"));
		cout << "--- unload" << endl;
		chara->unload();
		score->unload();
		cout << "--- shape sprite" << endl;
		cout << "\t" << sprite1->size << endl;
		cout << "\t" << sprite1->cellSize << endl;
		cout << "\t" << sprite1->anchor << endl;
		cout << "\t" << sprite1->flip << endl;
		cout << "\t" << sprite1->code << endl;
		cout << "--- shape text" << endl;
		cout << "\t" << text1->size << endl;
		cout << "\t" << text1->cellSize << endl;
		cout << "\t" << text1->blitSize << endl;
		cout << "\t" << text1->alignment << endl;
		cout << "\t" << text1->lines << endl;
		cout << "--- dtor" << endl;
	}

	void test2() {
		cout << __FUNCTION__ << endl;
		auto chara = make_unique<Asset>(L"asset/tutorial10/chara.json", AssetHandler::typeJson);
		auto img = make_unique<Asset>(L"asset/tutorial10/p1.png", AssetHandler::typeJson);
		cout << "--- load" << endl;
		chara->load();
		img->load();
		auto renderer2d = make_unique<Renderer2d>([](auto &) {});
		renderer2d->setRenderer(new DummyRenderer());
		auto a = make_unique<Actor>(
			Actor::noAction,
			[&chara, &img](Actor &a) {
				a.importProps(chara->props());
				Transform::append()(a);
				Material::append(*img)(a);
				Shape::append<ShapeSprite>()(a);
			}
		);
		cout << "--- draw" << endl;
		a->getComponent<Shape>([&renderer2d, &a](auto &shape) {
			shape.draw(*renderer2d, *a);
		});
		cout << "--- unload" << endl;
		chara->unload();
		img->unload();
		cout << "--- dtor" << endl;
	}
}

extern "C" int tutorial10() {
	Tutorial10::test1();
	Tutorial10::test2();
	return 0;
}
