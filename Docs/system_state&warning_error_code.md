

## System State Code

| Mã | Tên trạng thái | Mô tả ngắn |
|---|---|---|
| 0x0000 | IDLE | Hệ thống đang ở trạng thái chờ |
| 0x0001 | WARNING | Hệ thống có cảnh báo |
| 0x0002 | FAULT | Có lỗi hệ thống, không hoạt động |
| 0x0003 | EMERGENCY_STOPPED | Đã kích hoạt dừng khẩn cấp |

## Error Code

| Bit | Mã lỗi (Hex) | Tên lỗi | Mô tả lỗi |
|---|---|---|---|
| 0 | 0x0001 | ERROR_DI1 | Lỗi tín hiệu đầu vào số 1 |
| 1 | 0x0002 | ERROR_DI2 | Lỗi tín hiệu đầu vào số 2 |
| 2 | 0x0004 | ERROR_DI3 | Lỗi tín hiệu đầu vào số 3 |
| 3 | 0x0008 | ERROR_DI4 | Lỗi tín hiệu đầu vào số 4 |
| 4 | 0x0010 | ERROR_AI1 | Lỗi tín hiệu analog 1 |
| 5 | 0x0020 | ERROR_AI2 | Lỗi tín hiệu analog 2 |
| 6 | 0x0040 | ERROR_AI3 | Lỗi tín hiệu analog 3 |
| 7 | 0x0080 | ERROR_AI4 | Lỗi tín hiệu analog 4 |