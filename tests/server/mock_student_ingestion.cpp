#include "mock_student_ingestion.h"

namespace hegadb::server::storage {

std::vector<Student> MockStudentIngestion::get_sample_students() {
    return {
        {1001, 20, "Computer Science", 3.8, 95.5},
        {1002, 21, "Electrical Eng", 3.6, 88.0},
        {1003, 19, "Mathematics", 3.9, 97.2},
        {1004, 22, "Physics", 3.5, 85.0},
        {1005, 20, "Computer Science", 3.7, 92.0}
    };
}

} // namespace hegadb::server::storage
