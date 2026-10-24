// =============================================================================
// metrics.hpp — Métricas Prometheus (texto /metrics)
// =============================================================================
#pragma once
#include <atomic>
#include <string>
#include <sstream>
#include <map>
#include <mutex>

namespace l1::node {

class Metrics {
public:
    static Metrics& instance() { static Metrics M; return M; }

    void inc(const std::string& name, uint64_t v = 1) {
        std::lock_guard lock(mu_);
        counters_[name] += v;
    }
    void set_gauge(const std::string& name, double v) {
        std::lock_guard lock(mu_);
        gauges_[name] = v;
    }

    std::string render_prometheus() const {
        std::lock_guard lock(mu_);
        std::ostringstream oss;
        for (const auto& [k, v] : counters_) {
            oss << "# TYPE " << k << " counter\n" << k << " " << v << "\n";
        }
        for (const auto& [k, v] : gauges_) {
            oss << "# TYPE " << k << " gauge\n" << k << " " << v << "\n";
        }
        return oss.str();
    }

private:
    mutable std::mutex mu_;
    std::map<std::string, uint64_t> counters_;
    std::map<std::string, double> gauges_;
};

}  // namespace l1::node
