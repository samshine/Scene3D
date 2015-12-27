
#include "precomp.h"
#include "game_screen_controller.h"
#include "Model/GameWorld/game_master.h"
#include "Model/GameWorld/client_player_pawn.h"

using namespace uicore;

GameScreenController::GameScreenController(std::string hostname, std::string port, bool host_game)
{
	set_cursor_hidden();

	FontDescription font_desc;
	font_desc.set_height(13.0f);
	font_desc.set_line_height(18.0f);
	font = Font::create(font_desc, "Resources/Fonts/LuckiestGuy/LuckiestGuy.ttf");

	FontDescription font_desc2;
	font_desc2.set_height(26.0f);
	font_desc2.set_line_height(38.0f);
	font2 = Font::create(font_desc2, "Resources/Fonts/LuckiestGuy/LuckiestGuy.ttf");

	FontDescription font_desc3;
	font_desc3.set_height(44.0f);
	font_desc3.set_line_height(48.0f);
	font3 = Font::create(font_desc3, "Resources/Fonts/LuckiestGuy/LuckiestGuy.ttf");

	slots.connect(LogEvent::sig_log_event(), this, &GameScreenController::on_log_event);

	if (host_game)
		server_game = std::make_unique<GameWorld>(hostname, port);
	client_game = std::make_unique<GameWorld>(!hostname.empty() ? hostname : "localhost", port, std::make_shared<GameWorldClient>(window(), scene_engine(), sound_cache()));
}

void GameScreenController::on_log_event(const std::string &type, const std::string &text)
{
	log_messages.push_back(string_format("[%1] %2", type, text));
}

void GameScreenController::update()
{
	server_game->update(Vec2i());
	client_game->update(mouse_delta());

	scene_viewport()->set_camera(client_game->client->scene_camera);
	scene_viewport()->update(gc(), game_time().get_time_elapsed());
	scene_viewport()->render(gc());

	canvas()->begin();

	float offset = canvas()->height() - 22.0f - 180.0f - 50.0f;

	Path::rect(Rectf::xywh(11.0f, offset + 11.0f, 600.0f, 180.0f))->fill(canvas(), Brush::solid_rgba8(0, 0, 0, 50));
	Path::rect(Rectf::xywh(11.0f, offset + 11.0f, 600.0f, 1.0f))->fill(canvas(), Brush::solid_rgba8(200, 200, 255, 50));
	Path::rect(Rectf::xywh(11.0f, offset + 11.0f, 1.0f, 180.0f))->fill(canvas(), Brush::solid_rgba8(200, 200, 255, 50));
	Path::rect(Rectf::xywh(11.0f, offset + 191.0f, 600.0f, 1.0f))->fill(canvas(), Brush::solid_rgba8(200, 200, 255, 50));
	Path::rect(Rectf::xywh(611.0f, offset + 11.0f, 1.0f, 180.0f))->fill(canvas(), Brush::solid_rgba8(200, 200, 255, 50));

	auto font_metrics = font->font_metrics(canvas());
	auto font_metrics2 = font2->font_metrics(canvas());
	auto font_metrics3 = font3->font_metrics(canvas());

	float y = offset + 180.0f - font_metrics.line_height() + font_metrics.baseline_offset();
	for (size_t i = log_messages.size(); i > 0; i--)
	{
		const auto &msg = log_messages[i - 1];

		if (y - font_metrics.baseline_offset() < offset + 11.0f)
		{
			log_messages.erase(log_messages.begin(), log_messages.begin() + i);
			break;
		}

		font->draw_text(canvas(), 22.0f, y + 2.0f, msg, Colorf::black);
		font->draw_text(canvas(), 20.0f, y, msg, Colorf::whitesmoke);
		y -= font_metrics.line_height();
	}

	auto player = client_game->game_master->client_player;
	if (player)
	{
		std::string health_text = string_format("Health: %1%%", (int)std::ceil(player->get_health()));
		std::string armor_text = string_format("Armor: %1%%", (int)std::ceil(player->get_armor()));
		std::string weapon1_text = "Ice Launcher";
		std::string weapon2_text = "Ammo: 100";

		font2->draw_text(canvas(), 12.0f, 12.0f + font_metrics2.baseline_offset(), health_text, Colorf::black);
		font2->draw_text(canvas(), 10.0f, 10.0f + font_metrics2.baseline_offset(), health_text, Colorf::springgreen);
		font2->draw_text(canvas(), 12.0f, 12.0f + font_metrics2.baseline_offset() + font_metrics2.line_height(), armor_text, Colorf::black);
		font2->draw_text(canvas(), 10.0f, 10.0f + font_metrics2.baseline_offset() + font_metrics2.line_height(), armor_text, Colorf::orangered);

		font2->draw_text(canvas(), canvas()->width() - 12.0f - font2->measure_text(canvas(), weapon1_text).advance.width, canvas()->height() - 12.0f - font_metrics2.line_height() * 2.0f + font_metrics2.baseline_offset(), weapon1_text, Colorf::black);
		font2->draw_text(canvas(), canvas()->width() - 10.0f - font2->measure_text(canvas(), weapon1_text).advance.width, canvas()->height() - 10.0f - font_metrics2.line_height() * 2.0f + font_metrics2.baseline_offset(), weapon1_text, Colorf::springgreen);
		font2->draw_text(canvas(), canvas()->width() - 12.0f - font2->measure_text(canvas(), weapon2_text).advance.width, canvas()->height() - 12.0f - font_metrics2.line_height() + font_metrics2.baseline_offset(), weapon2_text, Colorf::black);
		font2->draw_text(canvas(), canvas()->width() - 10.0f - font2->measure_text(canvas(), weapon2_text).advance.width, canvas()->height() - 10.0f - font_metrics2.line_height() + font_metrics2.baseline_offset(), weapon2_text, Colorf::orangered);
	}

	std::string score_text = string_format("Score: %1", client_game->game_master->score);

	font2->draw_text(canvas(), canvas()->width() - 12.0f - font2->measure_text(canvas(), score_text).advance.width, 12.0f + font_metrics2.baseline_offset(), score_text, Colorf::black);
	font2->draw_text(canvas(), canvas()->width() - 10.0f - font2->measure_text(canvas(), score_text).advance.width, 10.0f + font_metrics2.baseline_offset(), score_text, Colorf::whitesmoke);

	if (client_game->game_master->announcement_timeout > 0.0f)
	{
		auto announcement_text1 = client_game->game_master->announcement_text1;
		auto announcement_text2 = client_game->game_master->announcement_text2;
		font3->draw_text(canvas(), (canvas()->width() - font3->measure_text(canvas(), announcement_text1).advance.width) * 0.5f + 2.0f, canvas()->height() * 0.3f + 2.0f, announcement_text1, Colorf::black);
		font3->draw_text(canvas(), (canvas()->width() - font3->measure_text(canvas(), announcement_text1).advance.width) * 0.5f, canvas()->height() * 0.3f, announcement_text1, Colorf::lightgoldenrodyellow);
		font2->draw_text(canvas(), (canvas()->width() - font2->measure_text(canvas(), announcement_text2).advance.width) * 0.5f + 2.0f, canvas()->height() * 0.3f + font_metrics3.line_height() + 2.0f, announcement_text2, Colorf::black);
		font2->draw_text(canvas(), (canvas()->width() - font2->measure_text(canvas(), announcement_text2).advance.width) * 0.5f, canvas()->height() * 0.3f + font_metrics3.line_height(), announcement_text2, Colorf::whitesmoke);
	}

	canvas()->end();
}
