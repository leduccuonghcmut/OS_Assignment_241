# Bài tập lớn môn hệ điều hành - Học kì 241
Bài tập này nhằm mô phỏng một hệ điều hành đơn giản, giúp sinh viên hiểu các khái niệm cơ bản về lập lịch, đồng bộ hóa và quản lý bộ nhớ. Hình 1 minh họa kiến trúc tổng quan của hệ điều hành sẽ được triển khai. Hệ điều hành phải quản lý hai tài nguyên ảo: CPU và RAM thông qua hai thành phần cốt lõi:
    - Bộ lập lịch (Scheduler) và Bộ phân phối (Dispatcher): Xác định tiến trình nào được phép chạy trên CPU nào.
    - Bộ nhớ ảo (Virtual memory): Tách biệt không gian bộ nhớ của từng tiến trình để chúng không nhận biết sự tồn tại của nhau. RAM vật lý được chia sẻ giữa các tiến trình, nhưng mỗi tiến trình có không gian bộ nhớ ảo riêng. Bộ máy bộ nhớ ảo sẽ ánh xạ và chuyển đổi các địa chỉ ảo mà các tiến trình cung cấp sang địa chỉ vật lý tương ứng.
    
Thông qua các module này, hệ điều hành cho phép nhiều tiến trình do người dùng tạo ra chia sẻ và sử dụng các tài nguyên tính toán ảo. Vì vậy, trong bài tập này, chúng ta tập trung triển khai bộ lập lịch/bộ phân phối và bộ máy bộ nhớ ảo.
