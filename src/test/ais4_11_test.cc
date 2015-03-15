// Test parsing message 4 for basestations and message 11 for date response.

#include <memory>

#include "gtest/gtest.h"
#include "ais.h"

namespace libais {
namespace {

std::unique_ptr<Ais4_11> Init(const string &nmea_string) {
  const string body(GetBody(nmea_string));
  const int pad = GetPad(nmea_string);

  // TODO(schwehr): Switch to c++14 make_unique.
  std::unique_ptr<Ais4_11> msg(new Ais4_11(body.c_str(), pad));
  if (!msg || msg->had_error()) {
    return nullptr;
  }
  return msg;
}

void Validate(
    const Ais4_11 *msg,
    const int message_id,
    const int repeat_indicator,
    const int mmsi,
    const int year,
    const int month,
    const int day,
    const int hour,
    const int minute,
    const int second,
    const int position_accuracy,
    const float x,
    const float y,
    const int fix_type,
    const int transmission_ctl,
    const int spare,
    const bool raim) {
  ASSERT_NE(nullptr, msg);
  ASSERT_EQ(message_id, msg->message_id);
  ASSERT_EQ(repeat_indicator, msg->repeat_indicator);
  ASSERT_EQ(mmsi, msg->mmsi);
  ASSERT_EQ(year, msg->year);
  ASSERT_EQ(month, msg->month);
  ASSERT_EQ(day, msg->day);
  ASSERT_EQ(hour, msg->hour);
  ASSERT_EQ(minute, msg->minute);
  ASSERT_EQ(second, msg->second);
  ASSERT_EQ(position_accuracy, msg->position_accuracy);
  ASSERT_FLOAT_EQ(x, msg->x);
  ASSERT_FLOAT_EQ(y, msg->y);
  ASSERT_EQ(fix_type, msg->fix_type);
  ASSERT_EQ(transmission_ctl, msg->transmission_ctl);
  ASSERT_EQ(spare, msg->spare);
  ASSERT_EQ(raim, msg->raim);
}

void ValidateCommState(
    const Ais4_11 *msg,
    const int sync_state,
    const int slot_timeout,
    const bool received_stations_valid,
    const int received_stations,
    const bool slot_number_valid,
    const int slot_number,
    bool utc_valid,
    const int utc_hour,
    const int utc_min,
    const int utc_spare,
    const bool slot_offset_valid,
    const int slot_offset) {
  ASSERT_EQ(sync_state, msg->sync_state);
  ASSERT_EQ(slot_timeout, msg->slot_timeout);

  ASSERT_EQ(received_stations_valid, msg->received_stations_valid);
  if (received_stations_valid) {
    ASSERT_EQ(received_stations, msg->received_stations);
  }

  ASSERT_EQ(slot_number_valid, msg->slot_number_valid);
  if (slot_number_valid) {
    ASSERT_EQ(slot_number, msg->slot_number);
  }

  ASSERT_EQ(utc_valid, msg->utc_valid);
  if (utc_valid) {
    ASSERT_EQ(utc_hour, msg->utc_hour);
    ASSERT_EQ(utc_min, msg->utc_min);
    ASSERT_EQ(utc_spare, msg->utc_spare);
  }

  ASSERT_EQ(slot_offset_valid, msg->slot_offset_valid);
  if (slot_offset_valid) {
    ASSERT_EQ(slot_offset, msg->slot_offset);
  }
}

TEST(Ais4Test, Decode4PosAcc1) {
  // Verifies decode of Msg 4 with position accuracy of 1.
  std::unique_ptr<Ais4_11> msg = Init(
      "!AIVDM,1,1,,B,4h3Owoiuiq000rdhR6G>oQ?020S:,0*10,raishub,1342569600");

  Validate(
      msg.get(), 4, 3, 3669983, 2012, 7, 18, 0, 0, 0, 1, -74.10847473144531,
      40.60139465332031, 15, 0, 0, true);

  ValidateCommState(
      msg.get(), 0, 0, false, 0, false, 0, false, 0, 0, 0, true, 2250);
}

TEST(Ais4Test, Decode4UtcCommState) {
  // Verifies decode of Msg4 with UTC comm state.
  std::unique_ptr<Ais4_11> msg = Init(
      "!AIVDM,1,1,,A,402=3g1uiposjOP71jSQ1sA026sd,0*03,raishub,1342569609");

  Validate(
      msg.get(), 4, 0, 2311100, 2012, 7, 17, 23, 59, 50, 0,
      -6.9665184021, 62.0688743591, 1, 0, 0, true);

  ValidateCommState(
      msg.get(), 0, 1, false, 0, false, 0, true, 23, 59, false, 0, 0);
}

TEST(Ais4Test, Decode4SlotNumBadPos00) {
  // Verifies decode of Msg4 slot number and bad 0, 0 position.
  std::unique_ptr<Ais4_11> msg = Init(
      "!AIVDM,1,1,,B,402FhL0000Htt000000000000@08,0*71,raishub,1342569629");

  Validate(
      msg.get(), 4, 0, 2470000, 0, 0, 0, 24, 60, 60, 0, 0.0, 0.0, 0, 0, 0,
      false);

  ValidateCommState(
      msg.get(), 0, 4, false, 0, true, 8, false, 0, 0, 0, false, 0);
}

TEST(Ais4Test, Decode4SlotOffset) {
  // Verifies decode of Msg4 with slot offset.
  std::unique_ptr<Ais4_11> msg = Init(
      "!AIVDM,1,1,,A,402VqV1uiq00e1KAk8OJHbC020S:,0*07,raishub,1342569645");

  Validate(
      msg.get(), 4, 0, 2734488, 2012, 7, 18, 0, 0, 45, 0, 19.9400062561,
      54.8969230652,  3, 0, 0, true);

  ValidateCommState(
      msg.get(), 0, 0, false, 0, false, 0, false, 0, 0, 0, true, 2250);
}

TEST(Ais4Test, Decode4TransmissionCtl) {
  // Verifies decode of message 4 with transmission control of 1.
  std::unique_ptr<Ais4_11> msg = Init(
      "!AIVDM,1,1,,B,4025bviuiq12e0hUg6OO?UbP0<=G,0*22,raishub,1342573365");

  Validate(
      msg.get(), 4, 0, 2190075, 2012, 7, 18, 1, 2, 45, 0, 10.6145648956,
      55.0295829773, 10, 1, 0, false);

  ValidateCommState(
      msg.get(), 0, 3, true, 855, false, 0, false, 0, 0, 0, false, 0);
}

TEST(Ais4Test, Decode4SyncState1) {
  // Verifies decode of Msg4 sync state of 1.
  std::unique_ptr<Ais4_11> msg = Init(
      "!AIVDM,1,1,,B,403v7B0000000`Vhfh<qtso00d2A,0*43,raishub,1342569602");

  Validate(
      msg.get(), 4, 0, 4163400, 0, 0, 0, 0, 0, 0, 1, 120.315666199,
      22.5539989471, 7, 0, 0, false);

  ValidateCommState(
      msg.get(), 1, 3, true, 145, false, 0, false, 0, 0, 0, false, 0);
}

TEST(Ais4Test, Decode4SyncState2) {
  // Verifies decode of Msg4 with a sync state of 2.
  std::unique_ptr<Ais4_11> msg = Init(
      "!AIVDM,1,1,,A,4FSR2mGO0oWdj<:TRhEM1oqrAFdE,0*1F,raishub,1342589098");

  Validate(
      msg.get(), 4, 1, 439911125, 7664, 3, 15, 7, 44, 50, 0, 170.081420898,
      37.4928512573, 9, 1, 420, false);

  ValidateCommState(
      msg.get(), 2, 5, true, 11029, false, 0, false, 0, 0, 0, false, 0);
}

TEST(Ais4Test, Decode4SyncState3) {
  // Verifies decode of Msg4 with sync state 3.
  std::unique_ptr<Ais4_11> msg = Init(
      "!AIVDM,1,1,,B,4fBT7K`;RtT3wP42c2n0OgLS1hA=,0*3B,raishub,1342588546");

  Validate(
      msg.get(), 4, 2, 958990190, 8376, 11, 25, 4, 3, 63, 1, 0.882934987545,
      94.385383606, 12, 1, 48, false);

  ValidateCommState(
      msg.get(), 3, 4, false, 0, true, 1101, false, 0, 0, 0, false, 0);
}

TEST(Ais11Test, Decode11) {
  // Verifies decode of message 11 for UTC date and time response.
  std::unique_ptr<Ais4_11> msg = Init(
      "!AIVDM,1,1,,B,;028j>iuiq0DoO0ARF@EEmG008Pb,0*25,raishub,1342570856");

  Validate(
      msg.get(), 11, 0, 2241083, 2012, 7, 18, 0, 20, 55, 0, -13.9211549759,
      28.5447807312, 7, 0, 0, false);

  ValidateCommState(
      msg.get(), 0, 2, false, 0, true, 2090, false, 0, 0, 0, false, 0);
}

}  // namespace
}  // namespace libais
