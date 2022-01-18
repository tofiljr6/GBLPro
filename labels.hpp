#pragma once

typedef struct cond_label {
    long long start;
    long long go_to;

    cond_label(long long start, long long go_to) {
        this->start = start;
        this->go_to = go_to;
    }
} cond_label;
