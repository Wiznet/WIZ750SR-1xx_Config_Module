#include "configmoduleHandler.h"

volatile int8_t current_page = 0;                //Current Print Page
volatile int8_t current_cursor = 0;              //Current Print Cursor

volatile int8_t ssd1306_page_num = PAGE_FIRST;    //Current Entered Page
volatile int8_t ssd1306_line_num = 0;             //Current Entered Line
volatile int8_t ssd1306_print_channel = 0;        //Current Entered Ch

volatile int8_t draw_line_status = OFF;           //Line On/Off Flag

volatile int8_t config_status = IS_WAITING_FOR_CONFIG;  //Current Configuration Status 
volatile int8_t config_index_cnt = 0;             // Count Variable for Config Array Index
volatile uint8_t blink_delay_cnt = 0;

uint32_t prev_temp_config_data[16];
uint32_t temp_config_data[16];
char blink_config_data[16][10];

char strings[][MAX_STRING_LENGTH] = {"\0", "\0", "\0", "\0"};

const char str_top_menu[6][MAX_STRING_LENGTH] = {
    "1. Device Info",
    "2. Network Config",
    "3. Ch Setting",
    "4. Serial Port",
    "5. RESET",
    "6. add function"
};

const char str_device_info[4][MAX_STRING_LENGTH] = {
    "Ver:",
    "Name:",
    "Code:",
    "MC:"
};
const char str_network_config[2][MAX_STRING_LENGTH] = { //Network config
    "1. DHCP",
    "2. Static IP"
};

const char str_ip_config[4][MAX_STRING_LENGTH] = { //DHCP setting of Network config
    "IP:",
    "GW:",
    "SN:",
    "DNS:"
};

const char str_ch_config[4][MAX_STRING_LENGTH] = { //DHCP setting of Network config
    "OP MODE:",
    "Local Port:",
    "RM IP:",
    "RM PORT:"
};

const char str_serial_config[5][MAX_STRING_LENGTH] = {
    "BaudRate:",
    "data bit:",
    "parity:",
    "stop bit:",
    "flow ctrl:"
};

const char str_empty_function[2][MAX_STRING_LENGTH] = {
    "add function",
    "add function"
};

void Config_Module_Btn_Process(void){

    uint32_t baud_tbl[] = {300, 600, 1200, 1800, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, 115200, 230400, 460800};
    DevConfig *dev_config = get_DevConfig_pointer();
    int i = 0;
    
    switch (btn_rotary_encoder)
    {
        case RIGHT:                     //Down scroll
            switch (ssd1306_print_channel)
            {
            case SSD1306_PRINT_CH_0:    //Main Page Function Selection Cursor Down
                Ctrl_Decrease_Line(2, 1);           
                break;
                
            case SSD1306_PRINT_CH_1:                
                switch (ssd1306_page_num)
                {
                case PAGE_FIRST:
                    switch (ssd1306_line_num)
                    {
                    case LINE_FIRST:    //Device Information Page Cursor Down
                        Ctrl_Decrease_Line(1, 0);           
                        break;
                    case LINE_SECOND:   //DHCP & STATIC IP Setting Cursor Down 
                        Ctrl_Decrease_Line(1, 0);    
                        break;
                    case LINE_THIRD:    //Ch Settings 
                        switch (config_status)
                        {
                            case CONFIG_DATA_SELECTION: //Setting Elements Seletion Cursor Down 
                                snprintf(blink_config_data[config_index_cnt], sizeof(blink_config_data[config_index_cnt]), "%d", temp_config_data[config_index_cnt]);  
                                config_index_cnt--;
                                if(config_index_cnt < 0) config_index_cnt = 0;
                                break;

                            case CONFIG_IN_PROGRESS:    //A Data of Ch Setting Elements Decrease
                                temp_config_data[config_index_cnt]--;
                                break;

                            default:
                                break;
                        }
                        break;
                    default:
                        break;
                    }
                    break;

                case PAGE_SECOND:
                    switch (ssd1306_line_num)
                    {
                    case LINE_FIRST:    //Serial Port(UART)
                        switch (config_status)
                        {
                            case CONFIG_DATA_SELECTION: //Serial Port(UART) Elements Seletion Cursor Down  
                                if(config_index_cnt == 0){ 
                                snprintf(blink_config_data[config_index_cnt], sizeof(blink_config_data[config_index_cnt]), "%d", baud_tbl[temp_config_data[config_index_cnt]]);
                                }
                                else{ 
                                snprintf(blink_config_data[config_index_cnt], sizeof(blink_config_data[config_index_cnt]), "%d", temp_config_data[config_index_cnt]);
                                }   
                                config_index_cnt--;
                                if(config_index_cnt < 0) config_index_cnt = 0;
                                break;

                            case CONFIG_IN_PROGRESS:    //A Data of Serial Port(UART) Elements Decrease
                                temp_config_data[config_index_cnt]--;
                                if(config_index_cnt == 0){
                                    if(temp_config_data[config_index_cnt] > baud_460800) temp_config_data[config_index_cnt] = baud_460800;
                                }
                                else if(config_index_cnt == 1){
                                    if(temp_config_data[config_index_cnt] >= 1) temp_config_data[config_index_cnt] = 1;
                                }
                                else if(config_index_cnt == 2){
                                    if(temp_config_data[config_index_cnt] > parity_even) temp_config_data[config_index_cnt] = parity_even;
                                } 
                                else if(config_index_cnt == 3){
                                    if(temp_config_data[config_index_cnt] > stop_bit2) temp_config_data[config_index_cnt] = stop_bit2;
                                }
                                else if(config_index_cnt == 4){
                                    if(temp_config_data[config_index_cnt] > flow_reverserts) temp_config_data[config_index_cnt] = flow_reverserts;
                                }
                                break;

                            default:

                                break;
                        }
                        break;

                    case LINE_SECOND:
                                   //ADD FUNCTION
                        Ctrl_Decrease_Line(1, 0);           //OK ? BACK?
                        break;

                    case LINE_THIRD:
                                   //ADD FUNCTION
                        break;

                    default:
                        break;
                    }
                    break;

                default:
                    break;
                }
                break;
            
            case SSD1306_PRINT_CH_2:
                switch (ssd1306_line_num)
                {
                    case LINE_FIRST:    //Dhcp OK & BACK Selection
                        Ctrl_Decrease_Line(1, 0);           //OK ? BACK?
                        break;

                    case LINE_SECOND:   //STATIC Setting
                        switch (config_status)
                        {
                            case CONFIG_DATA_SELECTION: //Static Elements Seletion Cursor Down  
                                snprintf(blink_config_data[config_index_cnt], sizeof(blink_config_data[config_index_cnt]), "%d", temp_config_data[config_index_cnt]);   
                                config_index_cnt--;
                                if(config_index_cnt < 0) config_index_cnt = 0;
                            break;

                            case CONFIG_IN_PROGRESS:    //A Data of Static Elements Decrease
                                temp_config_data[config_index_cnt]--;
                                if(temp_config_data[config_index_cnt] > 255) temp_config_data[config_index_cnt] = 255;
                            break;

                        default:
                            break;
                        }
                       
                        break;
                    
                    default:
                        break;
                    }   
                break;
            
            default:
                break;
            }
        break;

        case LEFT:                     //Up scroll
            switch (ssd1306_print_channel)
            {
            case SSD1306_PRINT_CH_0:    //Main Page Function Selection Cursor Up 
                Ctrl_Increase_Line(2, 1);           
                break;
                
            case SSD1306_PRINT_CH_1:                
                switch (ssd1306_page_num)
                {
                case PAGE_FIRST:
                    switch (ssd1306_line_num)
                    {
                        case LINE_FIRST:    //Device Information Page Cursor Up
                            Ctrl_Increase_Line(1, 0);
                            break;

                        case LINE_SECOND:   //DHCP & STATIC IP Setting Cursor Up 
                            Ctrl_Increase_Line(1, 0);
                            break;

                        case LINE_THIRD:    //Ch Settings 
                            switch (config_status)
                            {
                                case CONFIG_DATA_SELECTION: //Setting Element Seletion Cursor Up 
                                    snprintf(blink_config_data[config_index_cnt], sizeof(blink_config_data[config_index_cnt]), "%d", temp_config_data[config_index_cnt]); 
                                    config_index_cnt++;
                                    if(config_index_cnt >= 7) config_index_cnt = 6;
                                    break;

                                case CONFIG_IN_PROGRESS:    //A Data of Ch Setting Elements Increase
                                    temp_config_data[config_index_cnt]++;
                                    break;

                                default:
                                    break;
                            }
                            break;

                        default:
                            break;
                        }
                    break;

                case PAGE_SECOND:
                    switch (ssd1306_line_num)
                    {
                        case LINE_FIRST:    //Serial Port(UART)
                            switch (config_status)
                            {
                                case CONFIG_DATA_SELECTION: //Serial Port(UART) Elements Seletion Cursor Up
                                    if(config_index_cnt == 0){ 
                                        snprintf(blink_config_data[config_index_cnt], sizeof(blink_config_data[config_index_cnt]), "%d", baud_tbl[temp_config_data[config_index_cnt]]);
                                    }
                                    else{ 
                                        snprintf(blink_config_data[config_index_cnt], sizeof(blink_config_data[config_index_cnt]), "%d", temp_config_data[config_index_cnt]);
                                    }   
                                    config_index_cnt++;
                                    if(config_index_cnt >= 5) config_index_cnt = 4;
                                    break;

                                case CONFIG_IN_PROGRESS:    //A Data of Serial Port(UART) Elements Increase
                                    temp_config_data[config_index_cnt]++;
                                    if(config_index_cnt == 0){
                                        if(temp_config_data[config_index_cnt] > baud_460800) temp_config_data[config_index_cnt] = baud_300;
                                    }
                                    else if(config_index_cnt == 1){
                                        if(temp_config_data[config_index_cnt] >= 1) temp_config_data[config_index_cnt] = 0;
                                    }
                                    else if(config_index_cnt == 2){
                                        if(temp_config_data[config_index_cnt] > parity_even) temp_config_data[config_index_cnt] = parity_none;
                                    } 
                                    else if(config_index_cnt == 3){
                                        if(temp_config_data[config_index_cnt] > stop_bit2) temp_config_data[config_index_cnt] = stop_bit1;
                                    }
                                    else if(config_index_cnt == 4){
                                        if(temp_config_data[config_index_cnt] > flow_reverserts) temp_config_data[config_index_cnt] = flow_none;
                                    }
                                    break;

                                default:
                                    break;
                            }
                            break;

                        case LINE_SECOND: 
                                Ctrl_Increase_Line(1, 0);           //OK ? BACK?
                            break;

                        case LINE_THIRD:
                                //ADD FUNCTION 
                            break;

                        default:
                            break;
                    }
                    break;

                default:
                    break;
                }
                break;
            
            case SSD1306_PRINT_CH_2: 
                    switch (ssd1306_line_num)
                    {
                    case LINE_FIRST:    //Dhcp OK & BACK Selection
                        Ctrl_Increase_Line(1, 0);           //OK ? BACK?
 
                        break;
                    case LINE_SECOND:   //STATIC Setting
                        switch (config_status)
                        {   
                            case CONFIG_DATA_SELECTION: //Static Elements Seletion Cursor Up  
                                snprintf(blink_config_data[config_index_cnt], sizeof(blink_config_data[config_index_cnt]), "%d", temp_config_data[config_index_cnt]);   //인덱스 변경되면 변경 전 데이터 그대로 출력 위해 다시 저장
                                config_index_cnt++;
                                if(config_index_cnt >= 16) config_index_cnt = 15;
                                break;

                            case CONFIG_IN_PROGRESS:    //A Data of Static Elements Increase
                                temp_config_data[config_index_cnt]++;
                                if(temp_config_data[config_index_cnt] > 255) temp_config_data[config_index_cnt] = 0;
                                break;

                            default:
                                break;
                        }
                       
                        break;
                    
                    default:
                        break;
                    }   
                break;
            
            default:
                break;
            }
        break;



        case OK:    //OK BUTTON(Enter & Determine)

            switch (ssd1306_print_channel)
            {
            case SSD1306_PRINT_CH_0:    //Main Page, Enter One of Functions 
                ssd1306_print_channel = SSD1306_PRINT_CH_1;

                ssd1306_page_num = current_page;
                ssd1306_line_num = current_cursor;
                current_page = PAGE_FIRST;
                current_cursor = LINE_FIRST;  
                break;
                
            case SSD1306_PRINT_CH_1:
                switch (ssd1306_page_num)
                {
                case PAGE_FIRST:
                    switch (ssd1306_line_num)
                    {
                    case LINE_FIRST:    //Device Information Page Back
                        ssd1306_print_channel = SSD1306_PRINT_CH_0;
                        break;

                    case LINE_SECOND:   //Determine DHCP & STATIC IP Setting 
                        ssd1306_print_channel = SSD1306_PRINT_CH_2;
                        ssd1306_line_num = current_cursor;   
                        break;

                    case LINE_THIRD:    //Enter Ch Settings 
                        current_page = PAGE_FIRST;                     
                        current_cursor = LINE_THIRD;                    
                        switch (config_status)
                        {
                            case CONFIG_DATA_SELECTION: //Determine Setting Element 
                                blink_delay_cnt = 0;
                                snprintf(blink_config_data[config_index_cnt], sizeof(blink_config_data[config_index_cnt]), "%d", temp_config_data[config_index_cnt]);
                                config_status = CONFIG_IN_PROGRESS;      
                                break;

                            case CONFIG_IN_PROGRESS:    //Determine Current Data of Ch Setting Elements & Back to previous state for other Setting Elements Seletion 
                                config_status = CONFIG_DATA_SELECTION;
                                break;

                            default:
                                config_status = IS_WAITING_FOR_CONFIG;
                                break;
                        }

                        break;
                   
                   
                   
                    default:
                        break;
                    }
                    break;

                case PAGE_SECOND:
                    switch (ssd1306_line_num)
                    {
                    case LINE_FIRST:    //Serial Port(UART)
                        current_page = PAGE_SECOND;                    
                        current_cursor = LINE_FIRST;                  

                        switch (config_status)
                        {
                            case CONFIG_DATA_SELECTION: //Determine Setting Element 
                                blink_delay_cnt = 0;
                                if(config_index_cnt == 0){ 
                                    snprintf(blink_config_data[config_index_cnt], sizeof(blink_config_data[config_index_cnt]), "%d", baud_tbl[temp_config_data[config_index_cnt]]);
                                }
                                else{ 
                                    snprintf(blink_config_data[config_index_cnt], sizeof(blink_config_data[config_index_cnt]), "%d", temp_config_data[config_index_cnt]);
                                }  
                                config_status = CONFIG_IN_PROGRESS;       
                                break;

                            case CONFIG_IN_PROGRESS:    //Determine Current Data of Serial Port(UART) Elements & Back to previous state for other Setting Elements Seletion 
                                config_status--;  
                                break;

                            default:
                                config_status = IS_WAITING_FOR_CONFIG;
                                break;
                        }
                        break;

                    case LINE_SECOND:       //RESET

                        if(current_cursor == 0){    //RESET OK
                            device_reboot();
                            ssd1306_print_channel = SSD1306_PRINT_CH_0;
                            current_page = PAGE_SECOND;
                            current_cursor = LINE_SECOND;
                            ssd1306_page_num = current_page;
                            ssd1306_line_num = current_cursor;
                        }
                        else{                       //RESET Back
                            ssd1306_print_channel = SSD1306_PRINT_CH_0;
                            current_page = PAGE_SECOND;
                            current_cursor = LINE_SECOND;
                            ssd1306_page_num = current_page;
                            ssd1306_line_num = current_cursor;
                        }                        
                        break;
                        
                    case LINE_THIRD:       //Add Function
                        ssd1306_print_channel = SSD1306_PRINT_CH_0;
                        current_page = PAGE_SECOND;
                        current_cursor = LINE_THIRD;
                        ssd1306_page_num = current_page;
                        ssd1306_line_num = current_cursor;
                        break;

                    default:
                        break;
                    }
                    break;

                default:
                    break;
                }
                break;
            
            case SSD1306_PRINT_CH_2:
                switch (ssd1306_line_num)
                {
                    case LINE_FIRST:    //Enter Dhcp OK & BACK Selection
                        if(current_cursor == 0){    //DHCP OK
                            dev_config->options.dhcp_use = 1;
                            process_dhcp();
                            flash_update_start();
                            save_DevConfig_to_storage();
                            flash_update_end();
                            device_reboot();
                            ssd1306_print_channel--;
                            current_page = PAGE_FIRST;                    
                            current_cursor = LINE_SECOND;                  
                            ssd1306_page_num = current_page;
                            ssd1306_line_num = current_cursor;
                        }
                        else{                       //DHCP Back
                            ssd1306_print_channel--;
                            current_page = PAGE_FIRST;                   
                            current_cursor = LINE_SECOND;                  
                            ssd1306_page_num = current_page;
                            ssd1306_line_num = current_cursor;    
                        }                        
                        blink_delay_cnt = 0; 
                        break;

                    case LINE_SECOND:   //Enter STATIC Setting
                        current_page = PAGE_FIRST;                      
                        current_cursor = LINE_SECOND;                
                        switch (config_status)
                        {
                            case CONFIG_DATA_SELECTION: //Determine Setting Element 
                                blink_delay_cnt = 0;
                                snprintf(blink_config_data[config_index_cnt], sizeof(blink_config_data[config_index_cnt]), "%d", temp_config_data[config_index_cnt]);
                                config_status = CONFIG_IN_PROGRESS;            
                                break;
                            case CONFIG_IN_PROGRESS:    //Determine Current Data of Static Elements & Back to previous state for other Setting Elements Seletion 
                                config_status--;  
                                break;
                            default:
                                config_status = IS_WAITING_FOR_CONFIG;
                                break;
                        }
                        break;
                    
                    default:
                        break;
                    }       
                break;
            
            default:
                break;
            }
            break;

        case BACK:  //BACK BUTTON
            
        switch (ssd1306_print_channel)
        {     
            case SSD1306_PRINT_CH_0:    //Main Page, Doing Notting
            break;

            case SSD1306_PRINT_CH_1:
                switch (ssd1306_page_num)
                {
                case PAGE_FIRST:
                    switch (ssd1306_line_num)
                    {
                    case LINE_FIRST:    //Device Information Page Back
                        ssd1306_print_channel--;
                        current_page = PAGE_FIRST;
                        current_cursor = LINE_FIRST; 
                        break;

                    case LINE_SECOND:   //Back on DHCP & STATIC IP Setting 
                        ssd1306_print_channel--;
                        current_page = PAGE_FIRST;
                        current_cursor = LINE_SECOND; 
                        break;

                    case LINE_THIRD:    //Back on Ch Settings 
                        switch (config_status)
                        {
                            case CONFIG_DATA_SELECTION: //Back on Ch Settings
                                 if(set_flag_on_data_change(prev_temp_config_data, temp_config_data, 7)){   //Before Back, Detect Changed Data
                                    dev_config->network_info[0].working_mode = temp_config_data[0];
                                    dev_config->network_info[0].local_port = temp_config_data[1];
                                    dev_config->network_info[0].remote_ip[0] = temp_config_data[2];
                                    dev_config->network_info[0].remote_ip[1] = temp_config_data[3];
                                    dev_config->network_info[0].remote_ip[2] = temp_config_data[4];
                                    dev_config->network_info[0].remote_ip[3] = temp_config_data[5];
                                    dev_config->network_info[0].remote_port = temp_config_data[6];
                                    flash_update_start();
                                    save_DevConfig_to_storage();
                                    flash_update_end();
                                    device_reboot();
                                }
                                ssd1306_print_channel--;
                                blink_delay_cnt = 0;
                                current_page = PAGE_FIRST;
                                current_cursor = LINE_THIRD;
                                config_index_cnt = 0; 
                                config_status = IS_WAITING_FOR_CONFIG;            
                                break;

                            case CONFIG_IN_PROGRESS:    //Cancel Changed Data 
                                for(i = 0; i < 7; i++){
                                    temp_config_data[i] = prev_temp_config_data[i];
                                }
                                config_status = CONFIG_DATA_SELECTION;  
                                break;

                            default:
                                config_status = IS_WAITING_FOR_CONFIG;
                                break;
                        }
                        break;

                    default:
                        break;
                    }
                    break;

                case PAGE_SECOND:
                    switch (ssd1306_line_num)
                    {
                    case LINE_FIRST:    //Serial Port(UART)
                        switch (config_status)
                        {
                            case CONFIG_DATA_SELECTION: //Back on Setting
                                if(set_flag_on_data_change(prev_temp_config_data, temp_config_data, 5)){    //Before Back, Detect Changed Data
                                    dev_config->serial_info[0].baud_rate = temp_config_data[0];
                                    dev_config->serial_info[0].data_bits = temp_config_data[1];
                                    dev_config->serial_info[0].parity = temp_config_data[2];
                                    dev_config->serial_info[0].stop_bits = temp_config_data[3];
                                    dev_config->serial_info[0].flow_control = temp_config_data[4];
                                    flash_update_start();
                                    save_DevConfig_to_storage();
                                    flash_update_end();
                                    device_reboot();
                                    printf("\r\n change \r\n");
                                }
                                ssd1306_print_channel--;
                                blink_delay_cnt = 0;
                                current_page = PAGE_SECOND;
                                current_cursor = LINE_FIRST;
                                config_index_cnt = 0; 
                                config_status = IS_WAITING_FOR_CONFIG;        
                                break;
                            case CONFIG_IN_PROGRESS:    //Cancel Changed Data 
                                for(i = 0; i < 5; i++){
                                    temp_config_data[i] = prev_temp_config_data[i];
                                }
                                config_status = CONFIG_DATA_SELECTION;  
                                break;
                            default:
                                config_status = IS_WAITING_FOR_CONFIG;
                                break;
                        }
                        break;
                    case LINE_SECOND:   //ADD FUNCTION
                        ssd1306_print_channel = SSD1306_PRINT_CH_0;
                        current_page = PAGE_SECOND;
                        current_cursor = LINE_SECOND;
                        ssd1306_page_num = current_page;
                        ssd1306_line_num = current_cursor; 
                        break;
                    case LINE_THIRD:   //ADD FUNCTION
                        current_page = PAGE_SECOND; 
                        current_cursor = LINE_THIRD; 
                        break;
                    default:
                        break;
                    }
                    break;

                default:
                    break;
                }

                break;
            
            case SSD1306_PRINT_CH_2:
                    switch (ssd1306_line_num)
                    {
                    case LINE_FIRST:    //Back on Dhcp OK & BACK Selection
                        blink_delay_cnt = 0;
                        ssd1306_print_channel--;
                        current_page = PAGE_FIRST;                     
                        current_cursor = LINE_SECOND;                   
                        ssd1306_page_num = current_page;
                        ssd1306_line_num = current_cursor;                         
                        break;
                    case LINE_SECOND:   //Back on STATIC Setting
                        switch (config_status)
                        {
                            case CONFIG_DATA_SELECTION: //Back on Setting
                                if(set_flag_on_data_change(prev_temp_config_data, temp_config_data, 16)){    //Before Back, Detect Changed Data
                                    dev_config->options.dhcp_use = 0;
                                    dev_config->network_info_common.local_ip[0] = temp_config_data[0];
                                    dev_config->network_info_common.local_ip[1] = temp_config_data[1];
                                    dev_config->network_info_common.local_ip[2] = temp_config_data[2];
                                    dev_config->network_info_common.local_ip[3] = temp_config_data[3];
                                    dev_config->network_info_common.gateway[0] = temp_config_data[4];
                                    dev_config->network_info_common.gateway[1] = temp_config_data[5];
                                    dev_config->network_info_common.gateway[2] = temp_config_data[6];
                                    dev_config->network_info_common.gateway[3] = temp_config_data[7];
                                    dev_config->network_info_common.subnet[0] = temp_config_data[8];
                                    dev_config->network_info_common.subnet[1] = temp_config_data[9];
                                    dev_config->network_info_common.subnet[2] = temp_config_data[10];
                                    dev_config->network_info_common.subnet[3] = temp_config_data[11];
                                    dev_config->options.dns_server_ip[0] = temp_config_data[12];
                                    dev_config->options.dns_server_ip[1] = temp_config_data[13];
                                    dev_config->options.dns_server_ip[2] = temp_config_data[14];
                                    dev_config->options.dns_server_ip[3] = temp_config_data[15];
                                    flash_update_start();
                                    save_DevConfig_to_storage();
                                    flash_update_end();
                                    device_reboot();
                                }
                                ssd1306_print_channel--;
                                blink_delay_cnt = 0;
                                current_page = PAGE_FIRST;
                                current_cursor = LINE_SECOND;
                                config_index_cnt = 0; 
                                config_status = IS_WAITING_FOR_CONFIG;            
                                break;

                            case CONFIG_IN_PROGRESS:    //Cancel Changed Data 
                                for(i = 0; i < 16; i++){
                                    temp_config_data[i] = prev_temp_config_data[i];
                                }
                                config_status = CONFIG_DATA_SELECTION;  
                                break;

                            default:
                                config_status = IS_WAITING_FOR_CONFIG;
                                break;
                        }
                        break;

                    default:
                        break;
                    }      

                break;
            
            default:
                break;
            }
            ssd1306_page_num = current_page;
            ssd1306_line_num = current_cursor;   
            break;

        default:
            break;  
    }
    btn_rotary_encoder = 0;     //BTN Init
}



void SSD1306_PRINT_DATA_PROCESS(void){
    uint32_t baud_tbl[] = {300, 600, 1200, 1800, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, 115200, 230400, 460800};
    char strings[][MAX_STRING_LENGTH] = {"\0", "\0", "\0", "\0"};
    int i = 0;
    
    DevConfig *dev_config = get_DevConfig_pointer();

    switch (ssd1306_print_channel)
            {
            case SSD1306_PRINT_CH_0:
                draw_line_status = DRAW_LINE_ON;   
                switch (current_page)
                    {                 
                    case PAGE_FIRST:    
                        copyStringsRange(str_top_menu, strings, 0, 2);
                        break;

                    case PAGE_SECOND:           
                        copyStringsRange(str_top_menu, strings, 3, 5);
                        break;
                    default:
                        break;
                    }            
                break;
                
            case SSD1306_PRINT_CH_1:
                switch (ssd1306_page_num)
                {
                case PAGE_FIRST:
                    switch (ssd1306_line_num)
                    {
                    case LINE_FIRST:
                        draw_line_status = DRAW_LINE_OFF;
                        if(current_cursor == LINE_FIRST){
                        snprintf(strings[0], sizeof(strings[0]), "%s%d.%d.%d %s", str_device_info[0], dev_config->fw_ver[0], dev_config->fw_ver[1], dev_config->fw_ver[2], STR_VERSION_STATUS);
                        snprintf(strings[1], sizeof(strings[1]), "%s%s", str_device_info[1],dev_config->module_name);
                        snprintf(strings[2], sizeof(strings[2]), "%s%d.%d.%d", str_device_info[2],dev_config->module_type[0],dev_config->module_type[1],dev_config->module_type[2]);
                        snprintf(strings[3], sizeof(strings[3]), "%s%02X:%02X:%02X:%02X:%02X:%02X", str_device_info[3], dev_config->network_info_common.mac[0],dev_config->network_info_common.mac[1],
                                                                    dev_config->network_info_common.mac[2],dev_config->network_info_common.mac[3],dev_config->network_info_common.mac[4],dev_config->network_info_common.mac[5]);
                        }
                        else if(current_cursor == LINE_SECOND){

                        snprintf(strings[0], sizeof(strings[0]),"%s%d.%d.%d.%d", str_ip_config[0],dev_config->network_info_common.local_ip[0], dev_config->network_info_common.local_ip[1],
                                                    dev_config->network_info_common.local_ip[2], dev_config->network_info_common.local_ip[3]);
                        snprintf(strings[1], sizeof(strings[1]),"%s%d.%d.%d.%d", str_ip_config[1], dev_config->network_info_common.gateway[0], dev_config->network_info_common.gateway[1],
                                                    dev_config->network_info_common.gateway[2], dev_config->network_info_common.gateway[3]);
                        snprintf(strings[2], sizeof(strings[2]),"%s%d.%d.%d.%d", str_ip_config[2], dev_config->network_info_common.subnet[0], dev_config->network_info_common.subnet[1],
                                                    dev_config->network_info_common.subnet[2], dev_config->network_info_common.subnet[3]);
                        snprintf(strings[3], sizeof(strings[3]),"%s%d.%d.%d.%d", str_ip_config[3], dev_config->options.dns_server_ip[0], dev_config->options.dns_server_ip[1],
                                                    dev_config->options.dns_server_ip[2], dev_config->options.dns_server_ip[3]);
                        }
                        break;
                    case LINE_SECOND:
                        draw_line_status = DRAW_LINE_ON;
                        copyStringsRange(str_network_config, strings, 0, 1);                        
                        
                        break;
                    case LINE_THIRD: 
                        draw_line_status = DRAW_LINE_OFF;

                        switch (config_status)
                        {   
                            case IS_WAITING_FOR_CONFIG:
                                temp_config_data[0] = dev_config->network_info[0].working_mode;
                                temp_config_data[1] = dev_config->network_info[0].local_port;
                                temp_config_data[2] = dev_config->network_info[0].remote_ip[0];
                                temp_config_data[3] = dev_config->network_info[0].remote_ip[1];
                                temp_config_data[4] = dev_config->network_info[0].remote_ip[2];
                                temp_config_data[5] = dev_config->network_info[0].remote_ip[3];
                                temp_config_data[6] = dev_config->network_info[0].remote_port;
                                for(i = 0; i < 7; i++){
                                    snprintf(blink_config_data[i], sizeof(blink_config_data[i]), "%d", temp_config_data[i]);
                                    prev_temp_config_data[i] = temp_config_data[i];
                                }

                                config_status = CONFIG_DATA_SELECTION;
                                break;

                            case CONFIG_DATA_SELECTION:
                                blink_delay_cnt++;
                                if(blink_delay_cnt <= 6){

                                    snprintf(blink_config_data[config_index_cnt], sizeof(blink_config_data[config_index_cnt]), "%d", temp_config_data[config_index_cnt]);
                                }
                                else{
                                    snprintf(blink_config_data[config_index_cnt], sizeof(blink_config_data[config_index_cnt]), "%c%c", 32,32);
                                    if(blink_delay_cnt == 12) blink_delay_cnt = 0;
                                }

                                break;
                            case CONFIG_IN_PROGRESS:
                                blink_delay_cnt++;
                                if(blink_delay_cnt <= 1){

                                    snprintf(blink_config_data[config_index_cnt], sizeof(blink_config_data[config_index_cnt]), "%d", temp_config_data[config_index_cnt]);

                                }
                                else{

                                    snprintf(blink_config_data[config_index_cnt], sizeof(blink_config_data[config_index_cnt]), "%c%c", 32,32);
                                    if(blink_delay_cnt == 2) blink_delay_cnt = 0;
                                }
                        
                                break;
                            default:
                                break;
                        }
                        snprintf(strings[0], sizeof(strings[0]), "%s%s", str_ch_config[0],blink_config_data[0]);
                        snprintf(strings[1], sizeof(strings[1]), "%s%s", str_ch_config[1],blink_config_data[1]);
                        snprintf(strings[2], sizeof(strings[2]), "%s%s.%s.%s.%s", str_ch_config[2],blink_config_data[2], blink_config_data[3], blink_config_data[4], blink_config_data[5]);
                        snprintf(strings[3], sizeof(strings[3]), "%s%s", str_ch_config[3],blink_config_data[6]);
                        break;  

                    default:
                        break;
                    }                    
                    break;

                case PAGE_SECOND:
                draw_line_status = DRAW_LINE_OFF;
                    switch (ssd1306_line_num)
                    {
                    case LINE_FIRST:
                        draw_line_status = DRAW_LINE_OFF;

                        switch (config_status)
                        {   
                            case IS_WAITING_FOR_CONFIG: 
                                temp_config_data[0] = dev_config->serial_info[0].baud_rate;
                                temp_config_data[1] = dev_config->serial_info[0].data_bits;
                                temp_config_data[2] = dev_config->serial_info[0].parity;
                                temp_config_data[3] = dev_config->serial_info[0].stop_bits;
                                temp_config_data[4] = dev_config->serial_info[0].flow_control;
                                for(i = 0; i < 5; i++){
                                    if(i == 0){ 
                                    snprintf(blink_config_data[i], sizeof(blink_config_data[i]), "%d", baud_tbl[temp_config_data[i]]);
                                    }
                                    else{ 
                                    snprintf(blink_config_data[i], sizeof(blink_config_data[i]), "%d", temp_config_data[i]);
                                    }
                                    prev_temp_config_data[i] = temp_config_data[i];
                                }

                                config_status = CONFIG_DATA_SELECTION; 
                                break;

                            case CONFIG_DATA_SELECTION:
                                blink_delay_cnt++;
                                if(blink_delay_cnt <= 6){
                                    if(config_index_cnt == 0){ 
                                    snprintf(blink_config_data[config_index_cnt], sizeof(blink_config_data[config_index_cnt]), "%d", baud_tbl[temp_config_data[config_index_cnt]]);
                                    }
                                    else{ 
                                    snprintf(blink_config_data[config_index_cnt], sizeof(blink_config_data[config_index_cnt]), "%d", temp_config_data[config_index_cnt]);
                                    }
                                }
                                else{

                                    snprintf(blink_config_data[config_index_cnt], sizeof(blink_config_data[config_index_cnt]), "%c%c", 32,32);
                                    if(blink_delay_cnt == 12) blink_delay_cnt = 0;
                                }

                                break;
                            case CONFIG_IN_PROGRESS:
                                blink_delay_cnt++;
                                if(blink_delay_cnt <= 1){
                                    if(config_index_cnt == 0){ 
                                    snprintf(blink_config_data[config_index_cnt], sizeof(blink_config_data[config_index_cnt]), "%d", baud_tbl[temp_config_data[config_index_cnt]]);
                                    }
                                    else{ 
                                    snprintf(blink_config_data[config_index_cnt], sizeof(blink_config_data[config_index_cnt]), "%d", temp_config_data[config_index_cnt]);
                                    }
                                }
                                else{

                                    snprintf(blink_config_data[config_index_cnt], sizeof(blink_config_data[config_index_cnt]), "%c%c", 32,32);
                                    if(blink_delay_cnt == 2) blink_delay_cnt = 0;
                                }
                        
                                break;
                            default:
                                break;
                        }
                        if(config_index_cnt == 4){
                        snprintf(strings[0], sizeof(strings[0]), "%s%s", str_serial_config[4],blink_config_data[4]);
                        }
                        else{ 
                        
                        for(i = 0; i < 4; i++){
                        snprintf(strings[i], sizeof(strings[i]), "%s%s", str_serial_config[i], blink_config_data[i]);    
                        
                        }
                        }
                        break;
                    case LINE_SECOND:
                        blink_delay_cnt++;
                        snprintf(strings[0], sizeof(strings[0]), "%s", "YOU WANT TO RESET?");
                        snprintf(strings[1], sizeof(strings[1]), "%s", "    PRESS BUTTON");
                        if(current_cursor == 0){
                            if(blink_delay_cnt <= 6){
                                snprintf(strings[2], sizeof(strings[2]), "%s", "     [OK / BACK]");
                            }
                            else{
                                snprintf(strings[2], sizeof(strings[2]), "%s", "     [   / BACK]");
                                if(blink_delay_cnt == 12) blink_delay_cnt = 0;
                            }
                        }

                        else{
                            if(blink_delay_cnt <= 6){
                                snprintf(strings[2], sizeof(strings[2]), "%s", "     [OK / BACK]");
                            }
                            else{
                                snprintf(strings[2], sizeof(strings[2]), "%s", "     [OK /     ]");
                                if(blink_delay_cnt == 12) blink_delay_cnt = 0;
                            }
                        }
                        break;
                    case LINE_THIRD:
                        copyStringsRange(str_empty_function, strings, 1, 1); 
                        break;
                    default:
                        break;
                    }
                    break;
                default:
                    break;
                }
                break;
            
            case SSD1306_PRINT_CH_2:
                    draw_line_status = DRAW_LINE_OFF;
                   
                    switch (ssd1306_line_num)
                    {
                    case LINE_FIRST:
                        blink_delay_cnt++;
                        snprintf(strings[0], sizeof(strings[0]), "%s", "YOU WANT TO DO DHCP?");
                        snprintf(strings[1], sizeof(strings[1]), "%s", "    PRESS BUTTON");
                        if(current_cursor == 0){
                            if(blink_delay_cnt <= 6){
                                snprintf(strings[2], sizeof(strings[2]), "%s", "     [OK / BACK]");
                            }
                            else{
                                snprintf(strings[2], sizeof(strings[2]), "%s", "     [   / BACK]");
                                if(blink_delay_cnt == 12) blink_delay_cnt = 0;
                            }
                        }

                        else{
                            if(blink_delay_cnt <= 6){
                                snprintf(strings[2], sizeof(strings[2]), "%s", "     [OK / BACK]");
                            }
                            else{
                                snprintf(strings[2], sizeof(strings[2]), "%s", "     [OK /     ]");
                                if(blink_delay_cnt == 12) blink_delay_cnt = 0;
                            }
                        }
                        break;

                    case LINE_SECOND:
                        switch (config_status)
                        {   
                            case IS_WAITING_FOR_CONFIG:
                                temp_config_data[0] = dev_config->network_info_common.local_ip[0];
                                temp_config_data[1] = dev_config->network_info_common.local_ip[1];
                                temp_config_data[2] = dev_config->network_info_common.local_ip[2];
                                temp_config_data[3] = dev_config->network_info_common.local_ip[3];
                                temp_config_data[4] = dev_config->network_info_common.gateway[0];
                                temp_config_data[5] = dev_config->network_info_common.gateway[1];
                                temp_config_data[6] = dev_config->network_info_common.gateway[2];
                                temp_config_data[7] = dev_config->network_info_common.gateway[3];
                                temp_config_data[8] = dev_config->network_info_common.subnet[0];
                                temp_config_data[9] = dev_config->network_info_common.subnet[1];
                                temp_config_data[10] = dev_config->network_info_common.subnet[2];
                                temp_config_data[11] = dev_config->network_info_common.subnet[3];
                                temp_config_data[12] = dev_config->options.dns_server_ip[0];
                                temp_config_data[13] = dev_config->options.dns_server_ip[1];
                                temp_config_data[14] = dev_config->options.dns_server_ip[2];
                                temp_config_data[15] = dev_config->options.dns_server_ip[3];

                                for(i = 0; i < 16; i++){
                                    snprintf(blink_config_data[i], sizeof(blink_config_data[i]), "%d", temp_config_data[i]);
                                    prev_temp_config_data[i] = temp_config_data[i];
                                }

                                config_status = CONFIG_DATA_SELECTION;
                                break;

                            case CONFIG_DATA_SELECTION:
                                blink_delay_cnt++;
                                if(blink_delay_cnt <= 6){
                                    snprintf(blink_config_data[config_index_cnt], sizeof(blink_config_data[config_index_cnt]), "%d", temp_config_data[config_index_cnt]);
                                }
                                else{

                                    snprintf(blink_config_data[config_index_cnt], sizeof(blink_config_data[config_index_cnt]), "%c%c", 32,32);
                                    if(blink_delay_cnt == 12) blink_delay_cnt = 0;
                                }

                                break;
                            case CONFIG_IN_PROGRESS:
                                blink_delay_cnt++;
                                if(blink_delay_cnt <= 1){
                                    snprintf(blink_config_data[config_index_cnt], sizeof(blink_config_data[config_index_cnt]), "%d", temp_config_data[config_index_cnt]);
                                }
                                else{
                                    snprintf(blink_config_data[config_index_cnt], sizeof(blink_config_data[config_index_cnt]), "%c%c", 32,32);
                                    if(blink_delay_cnt == 2) blink_delay_cnt = 0;
                                }
                        
                                break;
                            default:
                                break;
                        }
                        for(i = 0; i < 4; i++){
                            snprintf(strings[i], sizeof(strings[i]), "%s%s.%s.%s.%s", str_ip_config[i],blink_config_data[(i*4)], blink_config_data[(i*4)+1], blink_config_data[(i*4)+2], blink_config_data[(i*4)+3]);
                        }
                        break;
                    
                    default:
                        break;
                    }
                break;
            
            default:
                break;
            }

            screen_update_function(strings, 4, current_cursor, draw_line_status);
}


uint8_t set_flag_on_data_change(const uint32_t* prev_array, const uint32_t* current_array, uint16_t array_size) {
    int i;
    for (i = 0; i < array_size; ++i) {
        if (prev_array[i] != current_array[i]) {
            return 1;  // Arrays are different
        }
    }
    return 0;  // Arrays are the same
}

//Cursor & Print Page Decrease Ctrl
void Ctrl_Decrease_Line(int8_t end_line, int8_t end_page){
    current_cursor--;
    if(current_cursor < 0 && current_page == 0){                       
        current_cursor = 0;                                                    
    }
    else if(current_cursor < 0 && current_page == end_page){                       
        current_page = 0;
        current_cursor = end_line;                                      
    }  
}

//Cursor & Print Page Increase Ctrl
void Ctrl_Increase_Line(int8_t end_line, int8_t end_page){
    current_cursor++;
    if(current_cursor > end_line && current_page == end_page){
            current_cursor = end_line;                                          
    }
    else if(current_cursor > end_line && current_page == 0){                           
            current_page = end_page;
            current_cursor = 0;
    }    
}
