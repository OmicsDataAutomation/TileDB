#!/bin/bash

if [[ $INSTALL_TYPE != basic ]]; then
	cd $TILEDB_BUILD_DIR && make examples && cd examples
	if [[ $INSTALL_TYPE == hdfs ]]; then
		$TRAVIS_BUILD_DIR/examples/run_examples.sh "hdfs://localhost:9000/travis_test";		
	elif [[ $INSTALL_TYPE == gcs ]]; then
		echo "GS_BUCKET=" $GS_BUCKET
		GOOGLE_APPLICATION_CREDENTIALS=$TRAVIS_BUILD_DIR/GCS.json $TRAVIS_BUILD_DIR/examples/run_examples.sh "gs://$GS_BUCKET/travis_test";
	fi
	diff travis_test.log $TRAVIS_BUILD_DIR/examples/expected_results;
fi
