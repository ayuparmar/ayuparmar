#ifndef SCRC_NTP_H
#define SCRC_NTP_H


void sync_time(int timeOffset);
uint64_t get_timestamp();
String get_timestamp_str(uint64_t timestamp);


#endif /* SCRC_NTP_H */
