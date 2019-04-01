pipeline {
    agent any

    // Test2

    stages {
        stage('build') {
            post {
                success {
                    slackSend color: 'good', message: "Build SUCCEEDED - ${env.JOB_NAME} ${env.BUILD_NUMBER} (<${env.BUILD_URL}|Open>)"
                    emailext subject: "${env.JOB_NAME} - Build # ${env.BUILD_NUMBER} - SUCCEEDED!", body: "${env.JOB_NAME} - Build # ${env.BUILD_NUMBER} - SUCCEEDED:\n\nCheck console output at ${env.BUILD_URL} to view the results.", to: "darkoppressor@gmail.com", attachLog: true
                }
                failure {
                    slackSend color: 'danger', message: "Build FAILED - ${env.JOB_NAME} ${env.BUILD_NUMBER} (<${env.BUILD_URL}|Open>)"
                    emailext subject: "${env.JOB_NAME} - Build # ${env.BUILD_NUMBER} - FAILED!", body: "${env.JOB_NAME} - Build # ${env.BUILD_NUMBER} - FAILED:\n\nCheck console output at ${env.BUILD_URL} to view the results.", to: "darkoppressor@gmail.com", attachLog: true
                }
                unstable {
                    slackSend color: 'warning', message: "Build UNSTABLE - ${env.JOB_NAME} ${env.BUILD_NUMBER} (<${env.BUILD_URL}|Open>)"
                    emailext subject: "${env.JOB_NAME} - Build # ${env.BUILD_NUMBER} - UNSTABLE!", body: "${env.JOB_NAME} - Build # ${env.BUILD_NUMBER} - UNSTABLE:\n\nCheck console output at ${env.BUILD_URL} to view the results.", to: "darkoppressor@gmail.com", attachLog: true
                }
            }

            steps {
                slackSend message: "Build started - ${env.JOB_NAME} ${env.BUILD_NUMBER} (<${env.BUILD_URL}|Open>)"

                sh '/home/tails/build-server/cheese-engine/tools/build-system/build $(pwd) true'
            }
        }
    }
}
