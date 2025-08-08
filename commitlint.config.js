export default {
  extends: ["@commitlint/config-angular"],
  rules: {
    "scope-empty": [2, "never"],
    "subject-empty": [2, "never"],
    "subject-max-length": [0],
    "body-leading-blank": [0],
    "footer-leading-blank": [0],
    "header-max-length": [0],
    "scope-case": [0],
    "subject-case": [0],
    "subject-full-stop": [0],
    "type-case": [0],
    "type-empty": [0],
  },
}
