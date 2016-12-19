/*
 * Copyright 2016 <copyright holder> <email>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "Timer.h"

void Timer::start()
{
    gettimeofday(&startTime, NULL);
}

double Timer::stop(std::string text)
{
    timeval endTime;
    long seconds, useconds;
    double duration;

    gettimeofday(&endTime, NULL);

    seconds  = endTime.tv_sec  - startTime.tv_sec;
    useconds = endTime.tv_usec - startTime.tv_usec;

    duration = seconds + useconds/1000000.0;
	if(duration >= 1)
	    std::cout << text << ": " << duration << " seconds" << std::endl;
	else
	    std::cout << text << ": " << duration*1000 << " miliseconds" << std::endl;
        return duration;
}

double Timer::getDelay()
{
    timeval endTime;
    long seconds, useconds;
    double duration;

    gettimeofday(&endTime, NULL);

    seconds  = endTime.tv_sec  - startTime.tv_sec;
    useconds = endTime.tv_usec - startTime.tv_usec;

    duration = seconds + useconds/1000000.0;
	return duration;
}
