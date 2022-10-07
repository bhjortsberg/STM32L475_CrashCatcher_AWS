
int erase_flash();
int has_data_flash();
int store_flash(uint8_t* buffer, uint32_t size);
const char* read_flash(uint32_t* size);
