class TopicEvolution:
    class Topic:
        def __init__(self, data):
            self.words: list[str] = [str(word) for word in data['topic']]
            self.id: int = data['id']
            self.health: float = data['health']

        def label(self, max_width=32):
            if len(self.words) == 0: return ''
            lines = [self.words[0]]
            for word in self.words[1:]:
                if len(lines[-1]) < max_width:
                    lines[-1] += ' ' + word
                else:
                    lines.append(word)
            return '\n'.join(lines)

    class Period:
        def __init__(self, data):
            if not data:
                self.topics = []
                return
            self.topics = [
                TopicEvolution.Topic(topic)
            for topic in data]

    def __init__(self, data):
        self.periods = [
            TopicEvolution.Period(period)
        for period in data]
