
//struct Rain {
//
//	point_t time_before_start_rain;
//
//	point_t time_rain_start;
//
//	point_t rain_duration;
//
//	bool is_raining;
//
//	Rain() {
//		is_raining = false;
//
//		std::uniform_int_distribution<u32> rnd_range(10, 20);
//
//		time_before_start_rain = rnd_range(rnd);
//
//		time_rain_start = 0;
//	}
//
//	bool is_full_raining() const {
//		return time_rain_start > 40;
//	}
//
//	void simulate(point_t delta_time) {
//		/*if (is_raining) {
//			if (time_rain_start > 40) {
//				rain_duration -= delta_time;
//				if (rain_duration < 0) {
//					is_raining = false;
//
//					std::uniform_int_distribution<u32> rnd_range(10, 20);
//
//					time_before_start_rain = rnd_range(rnd);
//
//					time_rain_start = 0;
//				}
//			}
//			else {
//				time_rain_start += delta_time;
//			}
//		}
//		else {
//			time_before_start_rain -= delta_time;
//			if (time_before_start_rain < 0) {
//				is_raining = true;
//				std::uniform_int_distribution<u32> rnd_range(50, 50);
//				rain_duration = rnd_range(rnd);
//			}
//		}*/
//	}
//
//	void draw() const {
//		/*if (is_raining) {
//			draw_rain(time_rain_start * 10, time_rain_start * 0.5);
//		}*/
//	}
//} rain;
