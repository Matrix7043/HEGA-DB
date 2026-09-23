#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace hegadb::server::storage {

struct Student {
    uint64_t student_id;
    uint32_t age;
    std::string major;
    double gpa;
    double exam_score;
};

class MockStudentIngestion {
public:
    static std::vector<Student> get_sample_students();
};

} // namespace hegadb::server::storage
