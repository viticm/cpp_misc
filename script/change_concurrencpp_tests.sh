#!/bin/sh

function copyfiles() {
  cp ~/develop/github/concurrencpp/test/source/tests/executor_tests ./ -r
  cp ~/develop/github/concurrencpp/test/source/tests/thread_tests ./ -r
  cp ~/develop/github/concurrencpp/test/source/tests/timer_tests ./ -r
}

copyfiles

curdir=`pwd`
cppfiles=`find ${curdir} -name "*.cpp"`

for file in $cppfiles
do
  newfile=`echo $file | sed 's;.cpp;.cc;g'`
  mv $file $newfile
done

files=`find ${curdir} -name "*.h" -o -name "*.cc"`

for file in $files
do
  sed -i 's;concurrencpp/concurrencpp.h;plain/all.h;g' $file
  sed -i 's;utils/object_observer.h;object_observer.h;g' $file
  sed -i 's;concurrencpp::details::thread;plain::thread;g' $file
  sed -i 's;concurrencpp::inline_executor;plain::concurrency::executor::Inline;g' $file
  sed -i 's;<inline_executor;<plain::concurrency::executor::Inline;g' $file
  sed -i 's;concurrencpp::manual_executor;plain::concurrency::executor::Manual;g' $file
  sed -i 's;<manual_executor;<plain::concurrency::executor::Manual;g' $file
  sed -i 's;concurrencpp::thread_executor;plain::concurrency::executor::Thread;g' $file
  sed -i 's;<thread_executor;<plain::concurrency::executor::Thread;g' $file
  sed -i 's;concurrencpp::thread_pool_executor;plain::concurrency::executor::ThreadPool;g' $file
  sed -i 's;<thread_pool_executor;<plain::concurrency::executor::ThreadPool;g' $file
  sed -i 's;concurrencpp::worker_thread_executor;plain::concurrency::executor::WorkerThread;g' $file
  sed -i 's;<worker_thread_executor;<plain::concurrency::executor::WorkerThread;g' $file
  sed -i 's;lazy_result<;concurrency::LazyResult<;g' $file
  sed -i 's;result<;concurrency::Result<;g' $file
  sed -i 's;result_promise<;concurrency::ResultPromise<;g' $file
  sed -i 's;shared_result<;concurrency::result::Shared<;g' $file

  sed -i 's;concurrencpp;plain;g' $file
  sed -i 's;    ;  ;g' $file
done
