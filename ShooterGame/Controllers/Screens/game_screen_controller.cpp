
#include "precomp.h"
#include "game_screen_controller.h"
#include "Model/GameWorld/game_master.h"
#include "Model/GameWorld/client_player_pawn.h"
#include "Model/Network/lock_step_client_time.h"

using namespace uicore;

GameScreenController::GameScreenController(std::string hostname, std::string port, bool host_game, float mouse_speed_x, float mouse_speed_y)
{
	set_cursor_hidden();

	FontDescription font_desc;
	font_desc.set_height(13.0f);
	font_desc.set_line_height(18.0f);
	font_desc.set_subpixel(false);
	font = Font::create(font_desc, "Resources/Fonts/LuckiestGuy/LuckiestGuy.ttf");

	FontDescription font_desc2;
	font_desc2.set_height(26.0f);
	font_desc2.set_line_height(38.0f);
	font_desc2.set_subpixel(false);
	font2 = Font::create(font_desc2, "Resources/Fonts/LuckiestGuy/LuckiestGuy.ttf");

	FontDescription font_desc3;
	font_desc3.set_height(44.0f);
	font_desc3.set_line_height(48.0f);
	font_desc3.set_subpixel(false);
	font3 = Font::create(font_desc3, "Resources/Fonts/LuckiestGuy/LuckiestGuy.ttf");

	FontDescription font_small_desc;
	font_small_desc.set_height(18.0f);
	font_small_desc.set_line_height(18.0f);
	font_small = Font::create("Consolas", font_small_desc);

	crosshair = Image::create(canvas(), "Resources/UI/HUD/crosshair3_red.png");

	slots.connect(LogEvent::sig_log_event(), this, &GameScreenController::on_log_event);

	if (host_game)
	{
		server_game = std::make_unique<GameWorld>(hostname, port);
		server_thread = std::thread(&GameScreenController::server_thread_main, this);
	}
	client_game = std::make_unique<GameWorld>(!hostname.empty() ? hostname : "localhost", port, std::make_shared<GameWorldClient>(window(), scene_engine(), sound_cache()));
	client_game->client->mouse_speed_x = mouse_speed_x;
	client_game->client->mouse_speed_y = mouse_speed_y;
}

GameScreenController::~GameScreenController()
{
	if (server_game)
	{
		server_mutex.lock();
		stop_server = true;
		server_mutex.unlock();
		server_thread.join();
	}
}

void GameScreenController::server_thread_main()
{
	try
	{
		while (true)
		{
			server_mutex.lock();
			if (stop_server)
			{
				server_mutex.unlock();
				break;
			}
			server_mutex.unlock();

			server_game->update(Vec2i(), false);
			System::sleep(5);
		}
	}
	catch (...)
	{
		server_exception = std::current_exception();
	}
}

void GameScreenController::on_log_event(const std::string &type, const std::string &text)
{
	log_messages.push_back(string_format("[%1] %2", type, text));
}

void GameScreenController::update()
{
	if (server_exception)
	{
		auto except = server_exception;
		server_exception = std::exception_ptr();
		std::rethrow_exception(except);
	}

	client_game->update(mouse_delta(), cursor_hidden());

	scene_viewport()->set_camera(client_game->client->scene_camera);
	scene_viewport()->update(gc(), game_time().time_elapsed());
	scene_viewport()->render(gc());

	canvas()->begin();

	float offset = canvas()->height() - 22.0f - 180.0f - 50.0f;

	Path::rect(Rectf::xywh(11.0f, offset + 11.0f, 600.0f, 180.0f))->fill(canvas(), Brush::solid_rgba8(0, 0, 0, 50));
	Path::rect(Rectf::xywh(11.0f, offset + 11.0f, 600.0f, 1.0f))->fill(canvas(), Brush::solid_rgba8(200, 200, 255, 50));
	Path::rect(Rectf::xywh(11.0f, offset + 11.0f, 1.0f, 180.0f))->fill(canvas(), Brush::solid_rgba8(200, 200, 255, 50));
	Path::rect(Rectf::xywh(11.0f, offset + 191.0f, 600.0f, 1.0f))->fill(canvas(), Brush::solid_rgba8(200, 200, 255, 50));
	Path::rect(Rectf::xywh(611.0f, offset + 11.0f, 1.0f, 180.0f))->fill(canvas(), Brush::solid_rgba8(200, 200, 255, 50));

	auto font_small_metrics = font_small->font_metrics(canvas());
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

	update_stats_cooldown = std::max(update_stats_cooldown - game_time().time_elapsed(), 0.0f);
	if (update_stats_cooldown == 0.0f)
	{
		fps = string_format("%1 fps", fps_counter);
		fps_counter = 0;

		update_stats.clear();
		for (const auto &result : client_game->client->scene_engine->gpu_results())
			update_stats.push_back(string_format("%1: %2 ms", result.name, (int)std::round(result.time_elapsed * 1000.0f)));

		update_stats_cooldown = 1.0f;

		ping = string_format("%1 ms ping", LockStepClientTime::actual_ping);
	}

	fps_counter++;

	y = 200.0f + font_small_metrics.baseline_offset();
	for (const auto &text : update_stats)
	{
		font_small->draw_text(canvas(), canvas()->width() - 12.0f - font_small->measure_text(canvas(), text).advance.width, y + 2.0f, text, Colorf::black);
		font_small->draw_text(canvas(), canvas()->width() - 10.0f - font_small->measure_text(canvas(), text).advance.width, y, text, Colorf::whitesmoke);
		y += font_small_metrics.line_height();
	}
	font_small->draw_text(canvas(), canvas()->width() - 12.0f - font_small->measure_text(canvas(), fps).advance.width, y + 2.0f, fps, Colorf::black);
	font_small->draw_text(canvas(), canvas()->width() - 10.0f - font_small->measure_text(canvas(), fps).advance.width, y, fps, Colorf::whitesmoke);
	y += font_small_metrics.line_height();
	font_small->draw_text(canvas(), canvas()->width() - 12.0f - font_small->measure_text(canvas(), ping).advance.width, y + 2.0f, ping, Colorf::black);
	font_small->draw_text(canvas(), canvas()->width() - 10.0f - font_small->measure_text(canvas(), ping).advance.width, y, ping, Colorf::whitesmoke);
	y += font_small_metrics.line_height();

	if (client_game->game_master->announcement_timeout > 0.0f)
	{
		auto announcement_text1 = client_game->game_master->announcement_text1;
		auto announcement_text2 = client_game->game_master->announcement_text2;
		float alpha = clamp(client_game->game_master->announcement_timeout, 0.0f, 1.0f);
		Colorf black = Colorf::black;
		Colorf color1 = Colorf::lightgoldenrodyellow;
		Colorf color2 = Colorf::whitesmoke;
		black.a = alpha * alpha;
		color1.a = alpha;
		color2.a = alpha;

		font3->draw_text(canvas(), (canvas()->width() - font3->measure_text(canvas(), announcement_text1).advance.width) * 0.5f + 2.0f, canvas()->height() * 0.3f + 2.0f, announcement_text1, black);
		font3->draw_text(canvas(), (canvas()->width() - font3->measure_text(canvas(), announcement_text1).advance.width) * 0.5f, canvas()->height() * 0.3f, announcement_text1, color1);
		font2->draw_text(canvas(), (canvas()->width() - font2->measure_text(canvas(), announcement_text2).advance.width) * 0.5f + 2.0f, canvas()->height() * 0.3f + font_metrics3.line_height() + 2.0f, announcement_text2, black);
		font2->draw_text(canvas(), (canvas()->width() - font2->measure_text(canvas(), announcement_text2).advance.width) * 0.5f, canvas()->height() * 0.3f + font_metrics3.line_height(), announcement_text2, color2);
	}

	crosshair->draw(canvas(), std::round((canvas()->width() - crosshair->width()) * 0.5f), std::round((canvas()->height() - crosshair->height()) * 0.5f));

	canvas()->end();
}
