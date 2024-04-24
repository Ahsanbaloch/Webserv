#!/usr/bin/python
print("""
    <style>
      form {
        width: 420px;
      }

      div {
        margin-bottom: 20px;
      }

      label {
        display: inline-block;
        width: 240px;
        text-align: right;
        padding-right: 10px;
      }

      button, input {
        float: right;
      }
    </style>
  </head>
  <body>
    <form method="post" enctype="application/x-www-form-urlencoded">
      <div>
        <label for="say">What greeting do you want to say?</label>
        <input name="say" id="say" value="Hi">
      </div>
      <div>
        <label for="to">Who do you want to say it to?</label>
        <input name="to" value="Mom">
      </div>
      <div>
        <button>Send my greetings</button>
      </div>
    </form>
  </body>
</html>""")